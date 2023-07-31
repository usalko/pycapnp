#pragma once

#include "kj/async.h"
#include "Python.h"
#include "capabilityHelper.h"

class PyEventPort: public kj::EventPort {
public:
  PyEventPort(PyObject * _py_event_port): py_event_port(_py_event_port) {
    // We don't need to incref/decref, since this C++ class will be owned by the Python wrapper class, and we'll make sure the python class doesn't refcount to 0 elsewhere.
    // Py_INCREF(py_event_port);
  }
  virtual bool wait() {
    GILAcquire gil;
    PyObject_CallMethod(py_event_port, const_cast<char *>("wait"), NULL);
    return true;  // TODO: get the bool result from python
  }

  virtual bool poll() {
    GILAcquire gil;
    PyObject_CallMethod(py_event_port, const_cast<char *>("poll"), NULL);
    return true;  // TODO: get the bool result from python
  }

  virtual void setRunnable(bool runnable) {
    GILAcquire gil;
    PyObject * arg = Py_False;
    if (runnable)
      arg = Py_True;
    PyObject_CallMethod(py_event_port, const_cast<char *>("set_runnable"), const_cast<char *>("o"), arg);
  }

private:
  PyObject * py_event_port;
};

void waitNeverDone(kj::WaitScope & scope) {
  GILRelease gil;
  kj::NEVER_DONE.wait(scope);
}

void pollWaitScope(kj::WaitScope & scope) {
  GILRelease gil;
  scope.poll();
}

kj::Timer * getTimer(kj::AsyncIoContext * context) {
  return &context->lowLevelProvider->getTimer();
}

void waitVoidPromise(kj::Promise<void> *promise, kj::WaitScope &scope)
{
  GILRelease gil;
  try
  {
    promise->wait(scope);
  }
  catch (kj::Exception& exn) {
    // auto obj = wrap_kj_exception_for_reraise(exn);
    // PyErr_SetObject((PyObject*)obj->ob_type, obj);
    // Py_DECREF(obj);
    // PyErr_SetString(PyExc_RuntimeError, exn.getDescription().cStr());
    PyErr_SetObject(PyExc_RuntimeError, exn.getDescription().cStr());
  }
  catch (const std::exception& exn) {
    // PyErr_SetString(PyExc_RuntimeError, exn.what());
    PyErr_SetObject(PyExc_RuntimeError, exn.what());
  }
  catch (...)
  {
    // PyErr_SetString(PyExc_RuntimeError, "Unknown exception");
    PyErr_SetObject(PyExc_RuntimeError, "Unknown exception");
  }
}

PyObject *waitPyPromise(kj::Promise<PyObject *> *promise, kj::WaitScope &scope)
{
  GILRelease gil;
  try
  {
    return promise->wait(scope);
  }
  catch (kj::Exception& exn) {
    // auto obj = wrap_kj_exception_for_reraise(exn);
    // PyErr_SetObject((PyObject*)obj->ob_type, obj);
    // Py_DECREF(obj);
    // PyErr_SetString(PyExc_RuntimeError, exn.getDescription().cStr());
    PyErr_SetObject(PyExc_RuntimeError, exn.getDescription().cStr());
    return NULL:
  }
  catch (const std::exception& exn) {
    // PyErr_SetString(PyExc_RuntimeError, exn.what());
    PyErr_SetObject(PyExc_RuntimeError, exn.what());
    return NULL:
  }
  catch (...)
  {
    // PyErr_SetString(PyExc_RuntimeError, "Unknown exception");
    PyErr_SetObject(PyExc_RuntimeError, "Unknown exception");
    return NULL:
  }
}

capnp::Response< ::capnp::DynamicStruct> * waitRemote(capnp::RemotePromise< ::capnp::DynamicStruct> * promise, kj::WaitScope & scope) {
  GILRelease gil;
  return new capnp::Response< ::capnp::DynamicStruct>(promise->wait(scope));
}

bool pollRemote(capnp::RemotePromise< ::capnp::DynamicStruct> * promise, kj::WaitScope & scope) {
  GILRelease gil;
  return promise->poll(scope);
}
