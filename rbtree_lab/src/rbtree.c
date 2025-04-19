#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void) {
  rbtree *t = (rbtree *)calloc(1, sizeof(rbtree)); // 동적 메모리 할당
  // TODO: initialize struct if needed
  // nil 노드 생성 & 초기화
  node_t *init = (node_t *)calloc(1, sizeof(node_t));
  // nil 은 black
  init->color = RBTREE_BLACK;
  // 초기상태 root노드 없으니까 nil
  t->root = init;
  t->nil = init;    // 이걸 안 하면 NULL 접근되거나 쓰레기 값 뜰 수 있음
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
  if (node == t->nil) return;
  delete_node(t, node->left);
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

  return t->root;
}


void rbtree_fix_up(rbtree *t, node_t *z){
  node_t *uncle;
  node_t *parent = z->parent; // 새 노드의 부모
  node_t *grand_parent = parent->parent; // 새 노드의 조부모
  // int is_uncle_right = 0;
  int is_straight_left = 0;
  int is_straight_right = 0;

  // 예외처리

  if (parent == t->nil || grand_parent == t->nil) return;
  // 1. 루트 노드 속성 위반
  if (t->root == z){
    z->color = RBTREE_BLACK; // 루트노드로 들어간 z를 새 노드로 변경
    return;
  }

  // 0. 삼촌 노드 방향 Tracking
  if (grand_parent->left == parent) {
    uncle = grand_parent->right;
  } else {
    uncle = grand_parent->left;
  }

  // 삽입 방향 체크
  if((parent->left == z) && (grand_parent->left == parent)){
    is_straight_left = 1;
  }
  if((parent->right == z) && (grand_parent->right == parent)){
    is_straight_right = 1;
  }

  // 새노드 z의 부모 색깔 확인
  if(parent->color == RBTREE_BLACK){ // 부모가 껌정이면 굳이 재조정 필요 없으니 return
    return;
  } 

  // CASE 1(부모 RED + 삼촌 RED)
  if(uncle != t->nil && uncle->color == RBTREE_RED){
    // parent & uncle와 grand_parent의 색 변경
    parent->color = RBTREE_BLACK;
    uncle->color = RBTREE_BLACK;
    grand_parent->color = RBTREE_RED;

    // 조부모 노드에서 다시 확인
    rbtree_fix_up(t, grand_parent); 
  } else{ // (부모 RED + 삼촌 BLACK)
  // CASE 3 (일직선)
    if(is_straight_left){ // 왼쪽 일직선
      // 1. 부모와 조부모 색 교환
      color_t temp = parent->color;
      parent->color = grand_parent->color;
      grand_parent->color = temp;
      // 2. 회전
      right_rotate(t, grand_parent);
    } else if(is_straight_right){
      // 1. 부모와 조부모 색 교환
      color_t temp = parent->color;
      parent->color = grand_parent->color;
      grand_parent->color = temp;

      left_rotate(t, grand_parent);
    } else {
      // CASE 2 (꺾인 선)
      if(parent->left == z){
        // 왼쪽 삽입인 경우
        right_rotate(t, parent);
        z = z->left;
      }
      else
      { // 오른쪽 삽입
        left_rotate(t, parent);
        z = z->right;
      }
      // CASE 3 적용
      parent = z->parent;
      grand_parent = parent->parent;
      
      color_t temp = parent->color;
      parent->color = grand_parent->color;
      grand_parent->color = temp;
      
      if (z == parent->left) {
        right_rotate(t, grand_parent);
      } else {
        left_rotate(t, grand_parent);
      }
    }
  }

  t->root->color = RBTREE_BLACK;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  // 1. 예외처리
  if(t == NULL || (t->root == t->nil)){
    return NULL;
  }

  // 2. 탐색 로직(반복문)
  node_t *current = t->root; 
  while (current != t->nil)
  {
    if(key == (current->key)){
      return current;
    } else if (key < (current->key)){
      current = current->left;
    } else {
      current = current->right;
    }
  }
  return NULL;
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

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
