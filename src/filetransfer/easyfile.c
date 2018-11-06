/*
 * Copyright (c) 2018 Elastos Foundation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>

#if defined(_WIN32) || defined(_WIN64)
#include <posix_helper.h>
#endif

#include <vlog.h>
#include <rc_mem.h>

#include <ela_carrier.h>
#include <ela_session.h>
#include "ela_filetransfer.h"
#include "easyfile.h"

#define _GNU_SOURCE

static
void notify_state_changed_cb(ElaFileTransfer *ft, FileTransferConnection state,
                             void *context)
{
    EasyFile *file = (EasyFile *)context;

    assert(file);
    assert(ft);

    if (state <= FileTransferConnection_initialized ||
        state > FileTransferConnection_closed)  {
        assert(0);
        vlogE("File: Invalid filetransfer connection state :%d.", state);
        return;
    }

    if (file->callbacks.state_changed)
        file->callbacks.state_changed(state, file->callbacks_context);
}

static
bool notify_file_cb(ElaFileTransfer *ft, const char *fileid,
                    const char *filename, uint64_t size, void *context)
{
    EasyFile *file = (EasyFile *)context;
    char fname[ELA_MAX_FILE_NAME_LEN + 1] = {0};
    char *p;
    int rc;

    assert(file);
    assert(ft);

    vlogD("File: Received filetransfer file event with info [%s:%s:%z].",
          fileid, filename, size);

    strcpy(file->fileid, fileid);
    file->filesz = size;

    rc = ela_filetransfer_pull(ft, fileid, file->offset);
    if (rc < 0)
        vlogE("File: Filetransfer pulling %s error (0x%x).", fileid,
              ela_get_error());

    return (rc >= 0);
}

static void *sending_file_routine(void *args)
{
    EasyFile *file = (EasyFile *)args;
    uint8_t buf[ELA_MAX_USER_DATA_LEN];
    uint64_t offset = file->offset;
    size_t send_len;
    int rc;

    rc = fseek(file->fp, offset, SEEK_SET);
    if (rc < 0) {
        vlogE("File: Seeking file %s to offset %llu error (%d).", file->fileid,
              offset, errno);
        return NULL;
    }

    do {
        send_len = file->filesz - offset;
        if (send_len > ELA_MAX_USER_DATA_LEN)
            send_len = ELA_MAX_USER_DATA_LEN;

        rc = fread(buf, send_len, 1, file->fp);
        if (rc < 0)  {
            vlogE("File: Reading file error (%d).", errno);
            //TODO: close?
            break;
        }

        rc = ela_filetransfer_send(file->ft, file->fileid, buf, send_len);
        if (rc < 0) {
            vlogE("File: Filetransfer sending %s error (0x%x).", file->fileid,
                  ela_get_error());
            //TOOD: close?
            break;
        }

        if (file->callbacks.sent)
            file->callbacks.sent(send_len, file->filesz, file->callbacks_context);

        offset += send_len;
    } while(offset < file->filesz);

    return NULL;
}

static
void notify_pull_cb(ElaFileTransfer *ft, const char *fileid, uint64_t offset,
                    void *context)
{
    EasyFile *file = (EasyFile *)context;
    char filename[ELA_MAX_FILE_NAME_LEN + 1] = {0};
    pthread_t thread;
    char *p;
    int rc;

    assert(file);
    assert(file->fp);
    assert(ft);

    vlogD("File: Received filetransfer pulling event for %s with offset %llu.",
          fileid, offset);

    if (offset >= file->filesz) {
        vlogE("File: Invalid filetransfer offset %llu to pull.", offset);
        //TODO: close ?
        return;
    }

    strcpy(file->fileid, fileid);
    file->offset = offset;

    rc = pthread_create(&thread, NULL, sending_file_routine, file);
    if (rc < 0) {
        vlogE("File: Creating backgroud thread to transferring file error.");
        return;
    }
    pthread_detach(thread);
}

static
bool notify_data_cb(ElaFileTransfer *ft, const char *fileid, const uint8_t *data,
                    size_t length, void *context)
{
    EasyFile *file = (EasyFile *)context;
    int rc;

    assert(file);
    assert(ft);

    vlogT("File: Received filetransfer data event from for %s with length %lu.",
          fileid, length);

    rc = fwrite(data, length, 1, file->fp);
    if (rc < 0) {
        vlogE("Fille: Writing data to file %s error (%d)", file->fileid, errno);
        return true;
    }

    file->offset += length;
    return (file->offset >= file->filesz);
}

static void easyfile_destroy(void *p)
{
    EasyFile *file = (EasyFile *)p;

    if (file->ft)  {
        ela_filetransfer_close(file->ft);
        file->ft = NULL;
    }

    if (file->fp) {
        fclose(file->fp);
        file->fp = NULL;
    }
}

int ela_file_send(ElaCarrier *w, const char *address, const char *filename,
                  ElaFileProgressCallbacks *callbacks, void *context)
{
    EasyFile *file;
    ElaFileTransferInfo fi;
    ElaFileTransferCallbacks cbs;
    struct stat st;
    char path[PATH_MAX] = {0};
    const char *p;
    int rc;

    if (!w || !address || !*address || !filename || !*filename || !callbacks) {
        ela_set_error(ELA_GENERAL_ERROR(ELAERR_INVALID_ARGS));
        return -1;
    }

    p = realpath(filename, path);
    if (!p) {
        ela_set_error(ELA_SYS_ERROR(errno));
        return -1;
    }

    p = basename(path);
    if (!p) {
        ela_set_error(ELA_SYS_ERROR(errno));
        return -1;
    }

    if (strlen(p) > ELA_MAX_FILE_NAME_LEN) {
        ela_set_error(ELA_GENERAL_ERROR(ELAERR_INVALID_ARGS));
        return -1;
    }

    rc = stat(path, &st);
    if (rc < 0) {
        ela_set_error(ELA_SYS_ERROR(errno));
        return -1;
    }

    file = (EasyFile *)rc_zalloc(sizeof(*file), easyfile_destroy);
    if (!file) {
        ela_set_error(ELA_GENERAL_ERROR(ELAERR_OUT_OF_MEMORY));
        return -1;
    }

    file->fp = fopen(path, "rb");
    if (!file->fp) {
        ela_set_error(ELA_SYS_ERROR(errno));
        deref(file);
        return -1;
    }

    file->callbacks = *callbacks;
    file->callbacks_context = context;
    file->filesz = st.st_size;
    file->offset = 0;

    memset(&fi, 0, sizeof(fi));
    strcpy(fi.filename, p);
    fi.size = st.st_size;

    memset(&cbs, 0, sizeof(cbs));
    cbs.state_changed = notify_state_changed_cb;
    cbs.pull = notify_pull_cb;

    file->ft = ela_filetransfer_new(w, address, &fi, &cbs, file);
    if (!file->ft) {
        vlogE("File: Creating filetransfer instance with info[%s] error (0x%x).",
              fi.filename, ela_get_error());
        deref(file);
        return -1;
    }

    rc = ela_filetransfer_connect(file->ft);
    if (rc < 0) {
        vlogE("File: Filetransfer connecting to %s error (0x%x).", address,
              ela_get_error());
        deref(file);
    }

    return rc;
}

int ela_file_recv(ElaCarrier *w, const char *address, const char *filename,
                  ElaFileProgressCallbacks *callbacks, void *context)
{
    EasyFile *file;
    ElaFileTransferCallbacks cbs;
    struct stat st;
    char path[PATH_MAX] = {0};
    char *p;
    int rc;

    if (!w || !address || !*address || !filename || !*filename || !callbacks) {
        ela_set_error(ELA_GENERAL_ERROR(ELAERR_INVALID_ARGS));
        return -1;
    }

    p = realpath(filename, path);
    if (!p) {
        ela_set_error(ELA_SYS_ERROR(errno));
        return -1;
    }

    rc = stat(path, &st);
    if (rc < 0) {
        ela_set_error(ELA_SYS_ERROR(errno));
        return -1;
    }

    file = (EasyFile *)rc_zalloc(sizeof(*file), easyfile_destroy);
    if (!file) {
        ela_set_error(ELA_GENERAL_ERROR(ELAERR_INVALID_ARGS));
        return -1;
    }

    file->callbacks = *callbacks;
    file->callbacks_context = context;
    file->filesz = 0;
    file->offset = st.st_size;

    file->fp = fopen(path, "wb");
    if (!file->fp) {
        ela_set_error(ELA_SYS_ERROR(errno));
        deref(file);
        return -1;
    }

    memset(&cbs, 0, sizeof(cbs));
    cbs.state_changed = notify_state_changed_cb;
    cbs.file = notify_file_cb;
    cbs.data = notify_data_cb;

    file->ft = ela_filetransfer_new(w, address, NULL, &cbs, file);
    if (!file->ft) {
        vlogE("File: Creating filetransfer instance to %s error (0x%x).",
              address, ela_get_error());
        deref(file);
        return -1;
    }

    rc = ela_filetransfer_accept_connect(file->ft);
    if (rc < 0) {
        vlogE("File: Accepting filletransfer connection from %s error (0x%x)",
              address, ela_get_error());
        deref(file);
    }

    return rc;
}
