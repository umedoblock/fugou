/* author: 梅濁酒(umedoblock)
 * Copyright 平成23-24(2011-2012)年
 */
#include "Python.h"
#include "structmember.h"

#include "libpar2.h"

#define PYPAR2_MAX_REDUNDANCY (1 << 13)
/* MO means Mega Octets(Bytes) */
/* ((2 ^ 13) ^ 2) * 4 = 64(= 2 ^ 26) * 4 = 256 Mega Octets */
/* 256 MO need for par2_big_bang()! */

#define Par2_MODULE

typedef struct {
    PyObject_HEAD
    /* Type-specific fields go here. */
    par2_t par2;
    size_t object_used_size;
} PyPar2Object;

#define PyPar2_Check(op) PyObject_TypeCheck(op, &PyPar2Type)
#define PyPar2_CheckExact(op) (Py_TYPE(op) == &PyPar2Type)

typedef struct {
    PyObject_HEAD
} PyBigBangObject;

/*
typedef struct PyModuleDef{
  PyModuleDef_Base m_base;
  const char* m_name;
  const char* m_doc;
  Py_ssize_t m_size;
  PyMethodDef *m_methods;
  inquiry m_reload;
  traverseproc m_traverse;
  inquiry m_clear;
  freefunc m_free;
}PyModuleDef;

typedef int (*inquiry)(PyObject *);
typedef int (*traverseproc)(PyObject *, visitproc, void *);
typedef void (*freefunc)(void *);
*/
