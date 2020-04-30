#include "fs_file.h"

#include "drivers/hdd.h"

File *FSFile_new(const char *path, u8 mode)
{
    FSEntry *file = getEntry(path);

    // Not found
    if (!file)
    {
        // TMP Create it
        return NULL;
    }
    
    FSFileData *data = malloc(sizeof(FSFileData));
    data->file = file;
    data->buf = Buffer_new(4 * HDD_SECTOR_SIZE);

    return File_new(data, FSFile_ops());
}

FileOps *FSFile_ops()
{
    FileOps *ops = malloc(sizeof(FileOps));

    *ops = (FileOps) {
        .read = FSFile_read,
        .write = FSFile_write,
        .close = FSFile_close,
    };

    return ops;
}

ssize_t FSFile_read(File *f, void *buffer, size_t count)
{
    // Read directly from the file
    return FSEntry_read(((FSFileData*) f->data)->file, buffer, count);
}

ssize_t FSFile_write(File *f, void *buffer, size_t count)
{
    return Buffer_write(((FSFileData*) f->data)->buf, buffer, count);
}

void FSFile_close(File *f)
{
    // Write to the file its content
    FSEntry_write(((FSFileData*) f->data)->file, ((FSFileData*) f->data)->buf->data, ((FSFileData*) f->data)->buf->size);

    // Delete
    Buffer_del(((FSFileData*) f->data)->buf);
    FSEntry_del(((FSFileData*) f->data)->file);
    free(f->data);
}
