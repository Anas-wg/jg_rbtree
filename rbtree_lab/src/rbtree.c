#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree)); // 동적 메모리 할당
  // TODO: initialize struct if needed
  // nil 노드 생성 & 초기화
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  // nil 은 black
  nil->color = RBTREE_BLACK;
  // 초기상태 root노드 없으니까 nil
  nil->left = nil->right = nil->parent = nil;
  t->root = nil;
  t->nil = nil;    // 이걸 안 하면 NULL 접근되거나 쓰레기 값 뜰 수 있음
  return t;
}

void left_rotate(rbtree *t, node_t *x){
  // 트리와 기준이 될 노드를 parameter로 받기
  node_t *y = x->right; // 오른쪽 자식 노드
  x->right = y->left; // Right Child의 왼쪽 서브 트리로 회전
  
  if (y->left != t->nil){ // 
    y->left->parent = x; // y의 왼쪽 서브 트리의 루트 => x(기준 노드)
  }
  y->parent = x->parent; // y의 부모가 x의 부모가 됨

  if(x->parent == t->nil){ // x가 루트 노드인 경우(부모 노드가 NIL)
    t->root = y; // 왼쪽 회전해서 올라온 y가 부모가 됨
  }else if (x == x->parent->left){ // x가 왼쪽 자식인 경우
    x->parent->left = y;           // y가 그자리로 들어감
  } else { // x가 오른쪽 자식인 경우
    x->parent->right = y; // y가 그 자리를 차지
  }
  y->left = x; // y의 왼쪽 자식을 x로 변경
  x->parent = y; // y를 x의 부모가 되게 upwards
}

void right_rotate(rbtree *t, node_t *x){
  node_t *y = x->left; // 왼쪽 자식 노드
  x->left = y->right; // 자식 포인터 교체
  if(y->right != t->nil){
    y->right->parent = x; // 부모 포인터 재설정
  }
  y->parent = x->parent;
  // 1. x가 루트 노드일 경우(루트 변경 처리)
  if(x->parent == t->nil){
    t->root = y; 
  } else if(x == x->parent->left){ 
    // 2. x가 왼쪽 자식인 경우
    x->parent->left = y;
  } else{
    // 3. x가 오른쪽 자식인 경우
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
}

void delete_node(rbtree *t, node_t *node) {
  if (node == t->nil || node == NULL) return;

  // 중복된 연결 방지
  if (node->left != t->nil)
    delete_node(t, node->left);
  if (node->right != t->nil)
    delete_node(t, node->right);

  free(node);
}


void delete_rbtree(rbtree *t) {
  if (t == NULL) return;
  delete_node(t, t->root);
  free(t->nil);
  free(t);
}


node_t *rbtree_insert(rbtree *t, const key_t key) {
  // TODO: implement insert
  node_t *x = t->root; // 현재 노드
  node_t *y = t->nil; // 삽입될 위치의 부모
  node_t *z = (node_t *)calloc(1, sizeof(node_t)); // key가 담길 새 노드 & 동적 할당
  z->key = key; // KEY 넣기
  // 1. 삽입 위치를 찾기(BST와 동일)
  while (x != t->nil)
  {
    y = x;
    if (z-> key < (x->key)){
      x = x->left;
    } else {
      x = x->right;
    }
  }
  z->parent = y;
  if (y == (t->nil)){ // 삽입된 위치의 부모가 없다 = 루트
    t->root = z;      // 루트 자리에 넣기
    // printf("Root set to: %d\n", key);
  } else if((z->key) < (y->key)) {
    y->left = z;  
  } else {
    y->right = z;
  }
  // 삽입된 새 노드 자식들은 NIL
  z->left = t->nil;
  z->right = t->nil;
  z->color = RBTREE_RED; // 삽입시 칼라는 레드

  rbtree_fix_up(t, z);
  return z;
}

void rbtree_fix_up(rbtree *t, node_t *z) {
  while (z->parent->color == RBTREE_RED) {
    node_t *parent = z->parent;
    node_t *grand_parent = parent->parent;

    node_t *uncle = (grand_parent->left == parent) ? grand_parent->right : grand_parent->left;

    if (uncle->color == RBTREE_RED) {
      // CASE 1: 부모와 삼촌이 모두 RED
      parent->color = RBTREE_BLACK;
      uncle->color = RBTREE_BLACK;
      grand_parent->color = RBTREE_RED;
      z = grand_parent;
    } else {
      // CASE 2 & 3
      if (parent == grand_parent->left) {
        if (z == parent->right) {
          // CASE 2: 좌-우 (꺾인 선)
          z = parent;
          left_rotate(t, z);
        }
        // CASE 3: 좌-좌 (일직선)
        parent = z->parent;
        grand_parent = parent->parent;
        parent->color = RBTREE_BLACK;
        grand_parent->color = RBTREE_RED;
        right_rotate(t, grand_parent);
      } else {
        if (z == parent->left) {
          // CASE 2: 우-좌 (꺾인 선)
          z = parent;
          right_rotate(t, z);
        }
        // CASE 3: 우-우 (일직선)
        parent = z->parent;
        grand_parent = parent->parent;
        parent->color = RBTREE_BLACK;
        grand_parent->color = RBTREE_RED;
        left_rotate(t, grand_parent);
      }
    }
  }
  t->root->color = RBTREE_BLACK;
}


// node_t *rbtree_find(const rbtree *t, const key_t key) {
//   // TODO: implement find
//   // 1. 예외처리
//   if(t == NULL || (t->root == t->nil)){
//     return NULL;
//   }
//   // 2. 탐색 로직(반복문)
//   node_t *current = t->root; 
//   node_t *result = NULL;
//   while (current != t->nil) {
//     printf("Find loop: at %d\n", current->key);  // ✅ 이거 추가
//     if(key == (current->key)){
//       result = current;
//       current = current->right;
//     }
//     else if (key < (current->key))
//     {
//       current = current->left;
//     }
//     else
//     {
//       current = current->right;
//     }
//   }
//   return result;
// }


#include <stdlib.h>

node_t *rbtree_find(const rbtree *t, const key_t key) {
  if (t == NULL || t->root == t->nil) return NULL;

  // 최대 10000개 삽입 대비
  const size_t stack_size = 20000;
  node_t **stack = malloc(sizeof(node_t *) * stack_size);
  if (stack == NULL) return NULL;  // malloc 실패 방어

  int top = -1;
  node_t *best = NULL;
  stack[++top] = t->root;

  while (top >= 0) {
    node_t *cur = stack[top--];
    if (cur == t->nil) continue;

    if (cur->key == key) best = cur;

    if (cur->right != t->nil) stack[++top] = cur->right;
    if (cur->left != t->nil)  stack[++top] = cur->left;
  }

  free(stack);
  return best;
}




node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
    // 1. 예외처리
  if(t == NULL || (t->root == t->nil)){
    return NULL;
  }

  node_t *current = t->root;
  while (current->left != t->nil)
  {
    current = current->left;
  }
  
  return current;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
    // 1. 예외처리
  if(t == NULL || (t->root == t->nil)){
    return NULL;
  }

  node_t *current = t->root;
  while (current->right != t->nil)
  {
    current = current->right;
  }
  
  return current;
}

node_t * tree_minimum(const rbtree *t, node_t * node){
  if (node == t->nil || node == NULL) {
      return t->nil; 
  }
  while (node->left != t->nil) {
        node = node->left;
  }
  return node;
} 

void rbtree_transplant(rbtree *t, node_t *u, node_t *v){
  // if (v == t->nil) {
  //   printf("Transplant: replacing %d with NIL\n", u->key);
  // } else {
  //   printf("Transplant: replacing %d with %d\n", u->key, v->key);
  // }

  if (u->parent == t->nil){
    t->root = v;
  } else if (u == u->parent->left){
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;

}

// 재조정 함수
void rbtree_delete_fixup(rbtree * t, node_t *x){
  node_t *eb = x;
  node_t *bro;
  int is_eb_left = 0;

  while (eb != t->root && eb->color == RBTREE_BLACK) {
    if(eb == t->nil){
      break;
    }
    // 방향 체크
    if(eb->parent->left == eb){
      is_eb_left = 1;
      bro = eb->parent->right;
    } else {
      is_eb_left = 0;
      bro = eb->parent->left;
    }

      // ✅ nil 형제 방지
    if (bro == t->nil) {
      eb = eb->parent;
      continue;
    }

    // CASE 1: DB형제 RED
    if (bro->color == RBTREE_RED){
      // DB 형제와 부모 노드 색교환
      color_t temp = bro->color;
      bro->color = bro->parent->color;
      bro->parent->color = temp;
      // 부모 노드 기준 회전
      if(is_eb_left){
        left_rotate(t, bro->parent);
        bro = eb->parent->right; 
      } else {
        right_rotate(t, bro->parent);
        bro = eb->parent->left;
      }
    }

    // CASE 2: 형제 BLACK + 자식 둘 다 BLACK
    if ((bro->left == t->nil || bro->left->color == RBTREE_BLACK) &&
        (bro->right == t->nil || bro->right->color == RBTREE_BLACK)){
          bro->color = RBTREE_RED; // DB 형제노드 색교환
          eb = eb->parent; // 부모에게 짬처리 
    }
    else {
      // CASE 3: 형제 BLACK, 오른쪽 자식만 RED
      // EB가 왼쪽
      if (is_eb_left && bro->right->color == RBTREE_BLACK){
        bro->left->color = RBTREE_BLACK;
        bro->color = RBTREE_RED;
        right_rotate(t, bro);
        bro = eb->parent->right;
      }
      // eb가 오른쪽
      else if (!is_eb_left && bro->left->color == RBTREE_BLACK){
        bro->right->color = RBTREE_BLACK;
        bro->color = RBTREE_RED;
        left_rotate(t, bro);
        bro = eb->parent->left;
      }

      // CASE 4: 형제 BLACK, 왼쪽 자식 RED
      bro->color = eb->parent->color;
      eb->parent->color = RBTREE_BLACK;
      if (is_eb_left){
        bro->right->color = RBTREE_BLACK;
        left_rotate(t, eb->parent);
      } else {
        bro->left->color = RBTREE_BLACK;
        right_rotate(t, eb->parent);
      }
      // break;
    }
  }

  // EB 노드 색 블랙으로 교환
  eb->color = RBTREE_BLACK;
}


int rbtree_erase(rbtree *t, node_t *z) {
  node_t *y = z; // 삭제될 노드 혹은 후임자
  node_t *x; // y의 원래 자리를 대체할 노드
  color_t y_original_color = y->color;
  // int should_free_z = 1;  // 기본적으로 free하되, 예외는 따로 처리


  // 자식이 1개 있는 CASE
  if (z->left == t->nil){ // 삭제 노드의 왼쪽 서브트리 X
    x = z->right; // 오른쪽을 대체 노드로 설정
    rbtree_transplant(t, z, z->right); // z자리에 z의 오른쪽 서브트리 이식
  } else if (z->right == t->nil){ // 삭제 노드의 오른쪽 서브트리 X
    x = z->left;
    rbtree_transplant(t, z, z->left);
  } else {
    // 자식 둘다 있는 경우
    y = tree_minimum(t, z->right); // 후임자 찾기(오른쪽 서브트리에서 가장 작은 값)
    y_original_color = y->color;   // 삭제되는 색, 후임자의 색으로 설정
    x = y->right;
    if (y != z->right){
      // y가 바로 오른쪽 자식이 아니고 더 내려가야 나오는 경우
      rbtree_transplant(t, y, y->right); // y 자리에 y의 오른쪽 서브트리로 교체
      y->right = z->right; // y의 오른쪽 서브트리를 z의 오른쪽 서브트리로 교체
      y->right->parent = y;
    } else {
      // if (x != t->nil)
        x->parent = y;
    }
      rbtree_transplant(t, z, y);
      y->left = z->left;
      y->left->parent = y;
      y->color = z->color;
      
      // should_free_z = 0;  // z는 y로 대체되었으므로 free하면 안 됨

  }
  if (y_original_color == RBTREE_BLACK)
  {
    rbtree_delete_fixup(t, x);
  }
  // if (should_free_z) {
  //   printf("Freeing node with key = %d\n", z->key);  // 🔍 추적용
  //   z->left = z->right = z->parent = t->nil;
  //   free(z);
  // }

  free(z);
  z = NULL;

  return 1;
}


// 중위 순회를 위한 재귀 함수
void inorder_traversal(const rbtree *t, node_t *node, key_t *arr, size_t *index, size_t n) {
  if (node == t->nil) {
    return;  // NIL 노드에 도달하면 종료
  }

  // 왼쪽 서브트리 순회
  inorder_traversal(t, node->left, arr, index, n);

  // 현재 노드의 키를 배열에 추가
  if (*index < n) {
    arr[*index] = node->key;
    (*index)++;
  }

  // 오른쪽 서브트리 순회
  inorder_traversal(t, node->right, arr, index, n);
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  if (t == NULL || arr == NULL) {
    return -1;  // NULL 포인터 체크
  }

  size_t index = 0;  // 배열의 인덱스

  // 루트부터 시작하여 중위 순회를 수행
  inorder_traversal(t, t->root, arr, &index, n);

  return 0;  // 성공적으로 완료
}

