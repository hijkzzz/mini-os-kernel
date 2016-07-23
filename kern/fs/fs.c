#include "fs.h"

fs_node_t *fs_root = NULL; // The root of the filesystem.

void init_fs()
{

}

uint32_t read_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
  // Has the node got a read callback?
  if (node->read != 0)
    return node->read(node, offset, size, buffer);
  else
    return 0;
}

uint32_t write_fs(fs_node_t *node, uint32_t offset, uint32_t size, uint8_t *buffer)
{
  if (node->write != 0)
    return node->write(node, offset, size, buffer);
  else
    return 0;
}

void open_fs(fs_node_t *node, uint8_t read, uint8_t write)
{
  if (node->open != 0)
    return node->open(node, read, write);
  else
    return 0;
}

void close_fs(fs_node_t *node)
{
  if (node->close != 0)
    return node->close(node);
  else
    return 0;
}

struct dirent *readdir_fs(fs_node_t *node, uint32_t index)
{
  if ((node->flags & 0x7) == FS_DIRECTORY && node->readdir != 0)
    return node->readdir(node, index);
  else
    return 0;
}

fs_node_t *finddir_fs(fs_node_t *node, char *name)
{
  if ((node->flags & 0x7) == FS_DIRECTORY && node->finddir != 0)
    return node->finddir(node, name);
  else
    return 0;
}

