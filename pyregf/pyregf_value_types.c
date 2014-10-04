/*
 * Python object definition of the libregf value types
 *
 * Copyright (C) 2009-2014, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <types.h>

#if defined( HAVE_STDLIB_H ) || defined( HAVE_WINAPI )
#include <stdlib.h>
#endif

#include "pyregf_value_types.h"
#include "pyregf_libregf.h"
#include "pyregf_python.h"
#include "pyregf_unused.h"

PyTypeObject pyregf_value_types_type_object = {
	PyObject_HEAD_INIT( NULL )

	/* ob_size */
	0,
	/* tp_name */
	"pyregf.value_types",
	/* tp_basicsize */
	sizeof( pyregf_value_types_t ),
	/* tp_itemsize */
	0,
	/* tp_dealloc */
	(destructor) pyregf_value_types_free,
	/* tp_print */
	0,
	/* tp_getattr */
	0,
	/* tp_setattr */
	0,
	/* tp_compare */
	0,
	/* tp_repr */
	0,
	/* tp_as_number */
	0,
	/* tp_as_sequence */
	0,
	/* tp_as_mapping */
	0,
	/* tp_hash */
	0,
	/* tp_call */
	0,
	/* tp_str */
	0,
	/* tp_getattro */
	0,
	/* tp_setattro */
	0,
	/* tp_as_buffer */
	0,
	/* tp_types */
	Py_TPFLAGS_DEFAULT,
	/* tp_doc */
	"pyregf value types object (wraps LIBREGF_COLUMN_TYPES)",
	/* tp_traverse */
	0,
	/* tp_clear */
	0,
	/* tp_richcompare */
	0,
	/* tp_weaklistoffset */
	0,
	/* tp_iter */
	0,
	/* tp_iternext */
	0,
	/* tp_methods */
	0,
	/* tp_members */
	0,
	/* tp_getset */
	0,
	/* tp_base */
	0,
	/* tp_dict */
	0,
	/* tp_descr_get */
	0,
	/* tp_descr_set */
	0,
	/* tp_dictoffset */
	0,
	/* tp_init */
	(initproc) pyregf_value_types_init,
	/* tp_alloc */
	0,
	/* tp_new */
	0,
	/* tp_free */
	0,
	/* tp_is_gc */
	0,
	/* tp_bases */
	NULL,
	/* tp_mro */
	NULL,
	/* tp_cache */
	NULL,
	/* tp_subclasses */
	NULL,
	/* tp_weaklist */
	NULL,
	/* tp_del */
	0
};

/* Initializes the type object
 * Returns 1 if successful or -1 on error
 */
int pyregf_value_types_init_type(
     PyTypeObject *type_object )
{
	if( type_object == NULL )
	{
		return( -1 );
	}
	type_object->tp_dict = PyDict_New();

	if( type_object->tp_dict == NULL )
	{
		return( -1 );
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "UNDEFINED",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_UNDEFINED ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "STRING",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_STRING ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "EXPANDABLE_STRING",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_EXPANDABLE_STRING ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "BINARY_DATA",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_BINARY_DATA ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "INTEGER_32BIT_LITTLE_ENDIAN",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_INTEGER_32BIT_LITTLE_ENDIAN ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "INTEGER_32BIT_BIG_ENDIAN",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_INTEGER_32BIT_BIG_ENDIAN ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "SYMBOLIC_LINK",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_SYMBOLIC_LINK ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "MULTI_VALUE_STRING",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_MULTI_VALUE_STRING ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "RESOURCE_LIST",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_RESOURCE_LIST ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "FULL_RESOURCE_DESCRIPTOR",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_FULL_RESOURCE_DESCRIPTOR ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "RESOURCE_REQUIREMENTS_LIST",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_RESOURCE_REQUIREMENTS_LIST ) ) != 0 )
	{
		goto on_error;
	}
	if( PyDict_SetItemString(
	     type_object->tp_dict,
	     "INTEGER_64BIT_LITTLE_ENDIAN",
	     PyInt_FromLong(
	      LIBREGF_VALUE_TYPE_INTEGER_32BIT_LITTLE_ENDIAN ) ) != 0 )
	{
		goto on_error;
	}
	return( 1 );

on_error:
	if( type_object->tp_dict != NULL )
	{
		Py_DecRef(
		 type_object->tp_dict );

		type_object->tp_dict = NULL;
	}
	return( -1 );
}

/* Creates a new value types object
 * Returns a Python object if successful or NULL on error
 */
PyObject *pyregf_value_types_new(
           void )
{
	pyregf_value_types_t *pyregf_value_types = NULL;
	static char *function                    = "pyregf_value_types_new";

	pyregf_value_types = PyObject_New(
	                      struct pyregf_value_types,
	                      &pyregf_value_types_type_object );

	if( pyregf_value_types == NULL )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize value types.",
		 function );

		goto on_error;
	}
	if( pyregf_value_types_init(
	     pyregf_value_types ) != 0 )
	{
		PyErr_Format(
		 PyExc_MemoryError,
		 "%s: unable to initialize value types.",
		 function );

		goto on_error;
	}
	return( (PyObject *) pyregf_value_types );

on_error:
	if( pyregf_value_types != NULL )
	{
		Py_DecRef(
		 (PyObject *) pyregf_value_types );
	}
	return( NULL );
}

/* Intializes a value types object
 * Returns 0 if successful or -1 on error
 */
int pyregf_value_types_init(
     pyregf_value_types_t *pyregf_value_types )
{
	static char *function = "pyregf_value_types_init";

	if( pyregf_value_types == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid value types.",
		 function );

		return( -1 );
	}
	return( 0 );
}

/* Frees a value types object
 */
void pyregf_value_types_free(
      pyregf_value_types_t *pyregf_value_types )
{
	static char *function = "pyregf_value_types_free";

	if( pyregf_value_types == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid value types.",
		 function );

		return;
	}
	if( pyregf_value_types->ob_type == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid value types - missing ob_type.",
		 function );

		return;
	}
	if( pyregf_value_types->ob_type->tp_free == NULL )
	{
		PyErr_Format(
		 PyExc_TypeError,
		 "%s: invalid value types - invalid ob_type - missing tp_free.",
		 function );

		return;
	}
	pyregf_value_types->ob_type->tp_free(
	 (PyObject*) pyregf_value_types );
}
