#pragma once
#include "Middle.hpp"
#include "Nodes.hpp"
#include <algorithm>
#include <vector>

struct B_star_tree {
  Node *root;
  Middle *middle;

  int insertContour(LinkListNode *cur, LinkListNode *cur_prev,
                    LinkListNode *cur_next) {
    int y = 0;
    while (cur_next && cur->x + cur->getWidth() > cur_next->x) {
      y = std::max(y, cur_next->y + cur_next->getHeight());
      if (cur->x + cur->getWidth() < cur_next->x + cur_next->getWidth())
        break;
      if (cur_next->prev)
        cur_next->prev->next = cur_next->next;
      if (cur_next->next)
        cur_next->next->prev = cur_next->prev;
      cur_next = cur_next->next;
    }
    cur->next = cur_next;
    if (cur->next)
      cur->next->prev = cur;
    cur->prev = cur_prev;
    if (cur->prev)
      cur->prev->next = cur;
    return y;
  }
  void dfs(Node *cur, LinkListNode *pa, int x, char type) {
    if (cur == nullptr)
      return;
    cur->x = x;
    if (type == 'R') {
      LinkListNode *pa_next = nullptr;
      if (pa)
        pa_next = pa->next;
      cur->y = insertContour(cur, pa, pa_next);
    } else {
      cur->y = insertContour(cur, pa->prev, pa);
    }
    dfs(cur->lc, cur, x + cur->getWidth(), 'R');
    dfs(cur->rc, cur, x, 'U');
  }

  std::vector<Node **> position;
  void getPosition(Node *cur, Node **forbidden, bool acceptEmpty) {
    if (cur == nullptr)
      return;
    if (&cur->lc != forbidden) {
      if (!acceptEmpty == bool(cur->lc))
        position.emplace_back(&cur->lc);
      getPosition(cur->lc, forbidden, acceptEmpty);
    }
    if (&cur->rc != forbidden) {
      if (!acceptEmpty == bool(cur->rc))
        position.emplace_back(&cur->rc);
      getPosition(cur->rc, forbidden, acceptEmpty);
    }
  }

  std::vector<Node **> &&getPosition(Node **forbidden = nullptr,
                                     bool acceptEmpty = false) {
    position = std::vector<Node **>();
    getPosition(root, forbidden, acceptEmpty);
    return std::move(position);
  }
  void traceRectPosition() { dfs(root, nullptr, 0, 'R'); }
  void swap_subtree(Node **a, Node **b) { std::swap(*a, *b); }
  void swap_node(Node *a, Node *b) {
    std::swap(a->rect, b->rect);
    std::swap(a->rotated, b->rotated);
    std::swap(a->x, b->x);
    std::swap(a->y, b->y);
    std::swap(middle->nodeMap[a->rect], middle->nodeMap[b->rect]);
  }
  void rotate_node(Node *a) { a->rotated = !a->rotated; }
};

struct BaseOperatorLog {
  virtual void undo() = 0;
  virtual ~BaseOperatorLog() {}
};

struct SwapSubtreeLog : public BaseOperatorLog {
  B_star_tree tree;
  Node **a;
  Node **b;
  SwapSubtreeLog(B_star_tree tree, Node **a, Node **b)
      : tree(tree), a(a), b(b) {}
  void undo() override { tree.swap_subtree(a, b); }
};
struct SwapNodeLog : public BaseOperatorLog {
  B_star_tree tree;
  Node *a;
  Node *b;
  SwapNodeLog(B_star_tree tree, Node *a, Node *b) : tree(tree), a(a), b(b) {}
  void undo() override { tree.swap_node(a, b); }
};
struct RotateNodeLog : public BaseOperatorLog {
  B_star_tree tree;
  Node *a;
  RotateNodeLog(B_star_tree tree, Node *a) : tree(tree), a(a) {}
  void undo() override { tree.rotate_node(a); }
};