#!/usr/bin/env python3
import math

class Node:
    """Node in a binary tree `Tree`"""

    def __init__(self, key, value, left=None, right=None, parent=None):
        self.key = key
        self.value = value
        self.parent = parent
        self.left = left
        self.right = right
        if left is not None: left.parent = self
        if right is not None: right.parent = self

class Tree:
    """A splay tree implementation"""

    def __init__(self, root=None):
        self.root = root

    def rotate(self, node):
        """ Rotate the given `node` up.

        Performs a single rotation of the edge between the given node
        and its parent, choosing left or right rotation appropriately.
        """
        if node.parent is not None:
            if node.parent.left == node:
                if node.right is not None: node.right.parent = node.parent
                node.parent.left = node.right
                node.right = node.parent
            else:
                if node.left is not None: node.left.parent = node.parent
                node.parent.right = node.left
                node.left = node.parent
            if node.parent.parent is not None:
                if node.parent.parent.left == node.parent:
                    node.parent.parent.left = node
                else:
                    node.parent.parent.right = node
            else:
                self.root = node
            node.parent.parent, node.parent = node, node.parent.parent

    def splay(self, node):
        """Splay the given node"""
        while node.parent is not None and node.parent.parent is not None:
            if (node.parent.right == node and node.parent.parent.right == node.parent) or \
                    (node.parent.left == node and node.parent.parent.left == node.parent):
                self.rotate(node.parent)
                self.rotate(node)
            else:
                self.rotate(node)
                self.rotate(node)
        if node.parent is not None:
            self.rotate(node)

    def lookup(self, key):
        """Look up the given key in the tree.

        Returns the node with the requested key or `None`.
        """
        node, node_last = self.root, None
        while node is not None:
            node_last = node
            if node.key == key:
                break
            if key < node.key:
                node = node.left
            else:
                node = node.right
        if node_last is not None:
            self.splay(node_last)
        return node

    def insert(self, key, value):
        """Insert (key, value) into the tree.

        If the key is already present, do nothing.
        """
        if self.root is None:
            self.root = Node(key, value)
            return

        node = self.root
        while node.key != key:
            if key < node.key:
                if node.left is None:
                    node.left = Node(key, value, parent=node)
                node = node.left
            else:
                if node.right is None:
                    node.right = Node(key, value, parent=node)
                node = node.right
        self.splay(node)

    def remove(self, key):
        """Remove given key from the tree.

        It the key is not present, do nothing.

        The implementation first splays the element to be removed to
        the root, and if it has both children, splays the largest element
        in the left subtree and links it to the original right subtree.
        """
        if self.lookup(key) is not None:
            right = self.root.right
            self.root = self.root.left
            if self.root is None:
                self.root, right = right, None
            if self.root is not None:
                self.root.parent = None

            if right is not None:
                node = self.root
                while node.right is not None:
                    node = node.right
                self.splay(node)
                self.root.right = right
                right.parent = self.root

    def range_sum(self, left, right):
        """Return number of elements in range [left, right]

        Given a closed range [left, right], return the sum of values of elements
        in the range, i.e., sum(value | (key, value) in tree, left <= key <= right).
        """
        raise NotImplementedError()
