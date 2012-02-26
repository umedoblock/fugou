#include "Python.h"
#include "structmember.h"

static PyObject *
Py_gcdext(PyObject *self, PyObject *args)
{
    PyObject *a = NULL, *b = NULL;
    PyObject *gcd = Py_None, *x = Py_None, *y = Py_None;

    if (!PyArg_ParseTuple(args, "OO", &a, &b))
        return NULL;

fprintf(stderr, "a =\n");
PyObject_Print(a, stderr, 0);
fprintf(stderr, "\n");

fprintf(stderr, "b =\n");
PyObject_Print(b, stderr, 0);
fprintf(stderr, "\n");

    return PyTuple_Pack(3, gcd, x, y);
}

static PyMethodDef gcdext_methods[] = {
    {"gcdext", (PyCFunction )Py_gcdext, METH_VARARGS, "gcdext()"},
    {NULL, NULL, 0, NULL}   /* sentinel */
};

static struct PyModuleDef gcdext_module = {
    PyModuleDef_HEAD_INIT,
    "_gcdext", /* name of module */
    NULL, /* module documentation */
    -1,
    gcdext_methods
};

PyMODINIT_FUNC
PyInit__gcdext(void)
{
    /* PyModuleObject *m; */
    PyObject *m;

    m = (PyObject *)PyModule_Create(&gcdext_module);
    if (m == NULL) {
        return NULL;
    }

    return m;
}
