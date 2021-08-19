#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
// Эту функцию мы вызываем из Python кода

void
matrix_update (PyObject *listMatrix1, long size)
{
  int m1size = PyList_Size (listMatrix1);
  for (int i = 0; i < size; i++)
    {
      if (m1size < size)
        PyList_Append (listMatrix1, PyList_New (0));
      else
        break;
    }
  for (int i = 0; i < size; i++)
    {
      PyObject *curr = PyList_GetItem (listMatrix1, i);
      for (int j = 0; j < size; ++j)
        {
          if (PyList_Size (curr) < size)
            PyList_Append (curr, Py_BuildValue ("i", 0));
          else
            break;
        }
    }
}

static PyObject *
matrix_dot (PyObject *self, PyObject *args)
{
  PyObject *listMatrix1, *listMatrix2;
  long size = 0;
  if (!PyArg_ParseTuple (args, "iOO", &size, &listMatrix1, &listMatrix2))
    {
      PyErr_SetString (PyExc_TypeError, "func_ret_str args error");
      return NULL;
    }
  int size1 = PyList_Size (listMatrix1);
  int size2 = PyList_Size (listMatrix2);
  int min_size = fmin (size1, size2);
  PyObject *result = PyList_New (0);

  for (long i = 0; i < min_size; ++i)
    {
      PyObject *firstMatrix = PyList_GetItem (listMatrix1, i);
      PyObject *tmpStr = PyList_New (0);
      for (long j = 0; j < min_size; ++j)
        {
          int tmp = 0;
          for (long k = 0; k < min_size; ++k)
            {
              PyObject *secondMatrix = PyList_GetItem (listMatrix2, k);
              tmp += PyLong_AsLong (PyList_GetItem (firstMatrix, k))
                     * PyLong_AsLong (PyList_GetItem (secondMatrix, j));
            }
          PyList_Append (tmpStr, Py_BuildValue ("i", tmp));
        }
      PyList_Append (result, tmpStr);
    }
  matrix_update (result, size);
  return result;
}

static char matrix_docs[] = "dot( ): Matrix multiplication\n";

static PyMethodDef matrix_funcs[]
    = { { "dot", (PyCFunction)matrix_dot, METH_VARARGS, matrix_docs },
        { NULL, NULL, 0, NULL } };

static struct PyModuleDef matrix
    = { PyModuleDef_HEAD_INIT, "matrix", "usage: matrix.dot()\n", -1,
        matrix_funcs };

PyMODINIT_FUNC
PyInit_matrix (void)
{
  return PyModule_Create (&matrix);
}