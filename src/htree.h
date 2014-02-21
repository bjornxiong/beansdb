#ifndef __HTREE_H__
#define __HTREE_H__

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <ctype.h>

inline static void* 
my_malloc(size_t s, const char *file, int line, const char *func) {
    void *p = malloc(s);
    if (p == NULL) {
        fprintf(stderr, "Out of memory: %d, %li bytes in %s (%s:%i)\n", errno, s, func, file, line);
        exit(1);
    }
    //memset(p, 0, s);
    return p;
}

#define malloc(X) my_malloc(X, __FILE__, __LINE__, __FUNCTION__)

typedef struct t_item Item;
struct t_item {
    /*
      int bucket = item->pos & 0xff; 表示是第几个文件  
      uint32_t pos = item->pos & 0xffffff00; 表示在文件中的位置
    */
    uint32_t pos;
    /*
     >0,该数据才有效，<0，数据无效
     ver不会等于0，因此如果set的参数为0时，表示是更新  
     ver不会等于-1，因此set的参数为-1时，表示是删除。  
     ver的更新方法见bitcast.c中的bc_set函数 
     */
    int32_t  ver;
    uint16_t hash;
    /*
      sizeof(Item) + n - ITEM_PADDING
      这个item的长度。通过这个长度找到下一个item 
    */
    uint8_t  length;
    /* key is a char-array , key is encoded by codec.c:dc_encode*/
    char     key[1]; 
};

#define ITEM_PADDING 1

typedef struct t_hash_tree HTree;
typedef void (*fun_visitor) (Item *it, void *param);

uint32_t fnv1a(const char *key, int key_len);

HTree*   ht_new(int depth, int pos);
void     ht_destroy(HTree *tree);
void     ht_add(HTree *tree, const char* key, uint32_t pos, uint16_t hash, int32_t ver);
void     ht_remove(HTree *tree, const char *key);
Item*    ht_get(HTree *tree, const char *key);
Item*    ht_get2(HTree *tree, const char *key, int ksz);
uint32_t ht_get_hash(HTree *tree, const char *key, int *count);
char*    ht_list(HTree *tree, const char *dir, const char *prefix);
void     ht_visit(HTree *tree, fun_visitor visitor, void *param);

HTree*     ht_open(int depth, int pos, const char *path);
int     ht_save(HTree *tree, const char *path);

// not thread safe
void     ht_add2(HTree *tree, const char* key, int ksz, uint32_t pos, uint16_t hash, int32_t ver);
void     ht_remove2(HTree *tree, const char *key, int ksz);

#endif /* __HTREE_H__ */
