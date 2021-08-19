#include <Python.h>

static PyObject *
primes_factor_out (PyObject *self, PyObject *args)
{

  int number;
  if (!PyArg_ParseTuple (args, "i", &number))
    {
      PyErr_SetString (PyExc_TypeError, "func_ret_str args error");
      return NULL;
    }
  if (number < 1)
    {
      PyErr_SetString (PyExc_TypeError, "func_ret_str args error");
      return NULL;
    }
  PyObject *answer = PyList_New (0);
  int d = 2;
  PyList_Append (answer, Py_BuildValue ("i", 1));
  while (number > 1)
    {
      while (number % d == 0)
        {
          number /= d;
          PyList_Append (answer, Py_BuildValue ("i", d));
        }
      d++;
    }
  if (PyList_Size (answer) == 2)
    {
      Py_XDECREF (answer);
      return Py_BuildValue ("s", "Prime!");
    }
  return answer;
}

static char primes_docs[] = "factor_out(): factorizing number\n";

static PyMethodDef primes_funcs[]
    = { { "factor_out", (PyCFunction)primes_factor_out, METH_VARARGS,
          primes_docs },
        { NULL, NULL, 0, NULL } };

static struct PyModuleDef primes_mod
    = { PyModuleDef_HEAD_INIT, "primes", "usage: Primes.factor_out(int a)\n",
        -1, primes_funcs };

PyMODINIT_FUNC
PyInit_primes (void)
{
  return PyModule_Create (&primes_mod);
}