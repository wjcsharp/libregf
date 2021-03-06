/*
 * Key item functions
 *
 * Copyright (C) 2009-2020, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libregf_debug.h"
#include "libregf_definitions.h"
#include "libregf_hive_bins_list.h"
#include "libregf_io_handle.h"
#include "libregf_key_item.h"
#include "libregf_libbfio.h"
#include "libregf_libcerror.h"
#include "libregf_libcnotify.h"
#include "libregf_libfcache.h"
#include "libregf_libfdata.h"
#include "libregf_libuna.h"
#include "libregf_named_key.h"
#include "libregf_security_key.h"
#include "libregf_unused.h"
#include "libregf_value_item.h"

#include "regf_cell_values.h"

/* Creates key item
 * Make sure the key_item is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_initialize(
     libregf_key_item_t **key_item,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_initialize";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( *key_item != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid key item value already set.",
		 function );

		return( -1 );
	}
	*key_item = memory_allocate_structure(
	             libregf_key_item_t );

	if( *key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create key item.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *key_item,
	     0,
	     sizeof( libregf_key_item_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear key item.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( *key_item != NULL )
	{
		memory_free(
		 *key_item );

		*key_item = NULL;
	}
	return( -1 );
}

/* Frees key item
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_free(
     libregf_key_item_t **key_item,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_free";
	int result            = 1;

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( *key_item != NULL )
	{
		if( ( *key_item )->named_key != NULL )
		{
			if( libregf_named_key_free(
			     &( ( *key_item )->named_key ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free the named key.",
				 function );

				result = -1;
			}
		}
		if( ( *key_item )->class_name != NULL )
		{
			memory_free(
			 ( *key_item )->class_name );
		}
		if( ( *key_item )->security_descriptor != NULL )
		{
			memory_free(
			 ( *key_item )->security_descriptor );
		}
		if( ( *key_item )->values_list != NULL )
		{
			if( libfdata_list_free(
			     &( ( *key_item )->values_list ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free the values data list.",
				 function );

				result = -1;
			}
		}
		if( ( *key_item )->values_cache != NULL )
		{
			if( libfcache_cache_free(
			     &( ( *key_item )->values_cache ),
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
				 "%s: unable to free values cache.",
				 function );

				result = -1;
			}
		}
		memory_free(
		 *key_item );

		*key_item = NULL;
	}
	return( result );
}

/* Reads a named key
 * Returns the number of bytes read if successful or -1 on error
 */
int libregf_key_item_read_named_key(
     libregf_key_item_t *key_item,
     libfdata_tree_node_t *key_tree_node,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     off64_t named_key_offset,
     uint32_t named_key_hash,
     libcerror_error_t **error )
{
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	static char *function                  = "libregf_key_item_read_named_key";
	int hive_bin_index                     = 0;
	int result                             = 0;

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( key_item->named_key != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid key item - named key value already set.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( hive_bins_list->io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid hive bins list - missing IO handle.",
		 function );

		return( -1 );
	}
	if( ( named_key_offset == 0 )
	 || ( named_key_offset >= (off64_t) 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid named key offset.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading named key at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 named_key_offset,
		 named_key_offset );
	}
#endif
	if( libregf_hive_bins_list_get_cell_at_offset(
	     hive_bins_list,
	     file_io_handle,
	     (uint32_t) named_key_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin cell at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 named_key_offset,
		 named_key_offset );

		goto on_error;
	}
	if( libregf_named_key_initialize(
	     &( key_item->named_key ),
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create named key.",
		 function );

		goto on_error;
	}
	if( libregf_named_key_read_data(
	     key_item->named_key,
	     hive_bin_cell->data,
	     hive_bin_cell->size,
	     named_key_hash,
	     hive_bins_list->io_handle->ascii_codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read named key at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 named_key_offset,
		 named_key_offset );

		goto on_error;
	}
	if( libregf_key_item_read_class_name(
	     key_item,
	     file_io_handle,
	     hive_bins_list,
	     key_item->named_key->class_name_offset,
	     key_item->named_key->class_name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read class name at offset: %" PRIu32 " (0x%08" PRIx32 ").",
		 function,
		 key_item->named_key->class_name_offset,
		 key_item->named_key->class_name_offset );

		goto on_error;
	}
	if( key_item->named_key->security_key_offset != 0xffffffffUL )
	{
		if( libregf_key_item_read_security_key(
		     key_item,
		     file_io_handle,
		     hive_bins_list,
		     key_item->named_key->security_key_offset,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read security key at offset: %" PRIu32 " (0x%08" PRIx32 ").",
			 function,
			 key_item->named_key->security_key_offset,
			 key_item->named_key->security_key_offset );

			goto on_error;
		}
	}
	if( key_item->named_key->number_of_sub_keys > 0 )
	{
		result = libfdata_tree_node_sub_nodes_data_range_is_set(
		          key_tree_node,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine if sub nodes data range is set.",
			 function );

			goto on_error;
		}
		else if( result == 0 )
		{
			result = libregf_hive_bins_list_get_index_at_offset(
			          hive_bins_list,
			          (off64_t) key_item->named_key->sub_keys_list_offset,
			          &hive_bin_index,
			          error );

			if( result == -1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to determine if sub keys list offset is valid.",
				 function );

				goto on_error;
			}
			else if( result == 0 )
			{
				key_item->item_flags |= LIBREGF_KEY_ITEM_FLAG_IS_CORRUPTED;
			}
			else
			{
				if( libfdata_tree_node_set_sub_nodes_data_range(
				     key_tree_node,
				     0,
				     (off64_t) key_item->named_key->sub_keys_list_offset,
				     0,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set sub keys list as sub nodes range.",
					 function );

					goto on_error;
				}
			}
		}
	}
/* TODO clone function */
	if( libfdata_list_initialize(
	     &( key_item->values_list ),
	     (intptr_t *) hive_bins_list,
	     NULL,
	     NULL,
	     (int (*)(intptr_t *, intptr_t *, libfdata_list_element_t *, libfdata_cache_t *, int, off64_t, size64_t, uint32_t, uint8_t, libcerror_error_t **)) &libregf_value_item_read_element_data,
	     NULL,
	     LIBFDATA_DATA_HANDLE_FLAG_NON_MANAGED,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create values data list.",
		 function );

		goto on_error;
	}
	if( libfcache_cache_initialize(
	     &( key_item->values_cache ),
	     LIBREGF_MAXIMUM_CACHE_ENTRIES_VALUES,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create values cache.",
		 function );

		goto on_error;
	}
	result = libregf_hive_bins_list_get_index_at_offset(
	          hive_bins_list,
	          (off64_t) key_item->named_key->values_list_offset,
	          &hive_bin_index,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to determine if values list offset is valid.",
		 function );

		goto on_error;
	}
	else if( result == 0 )
	{
		key_item->item_flags |= LIBREGF_KEY_ITEM_FLAG_IS_CORRUPTED;
	}
	else
	{
		if( libregf_key_item_read_values_list(
		     key_item,
		     file_io_handle,
		     hive_bins_list,
		     key_item->named_key->values_list_offset,
		     key_item->named_key->number_of_values,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_IO,
			 LIBCERROR_IO_ERROR_READ_FAILED,
			 "%s: unable to read values list at offset: %" PRIu32 " (0x%08" PRIx32 ").",
			 function,
			 key_item->named_key->values_list_offset,
			 key_item->named_key->values_list_offset );

			goto on_error;
		}
	}
	/* The values and sub keys are read on demand
	 */
	return( 1 );

on_error:
	if( key_item->values_cache != NULL )
	{
		libfcache_cache_free(
		 &( key_item->values_cache ),
		 NULL );
	}
	if( key_item->values_list != NULL )
	{
		libfdata_list_free(
		 &( key_item->values_list ),
		 NULL );
	}
	if( key_item->named_key != NULL )
	{
		libregf_named_key_free(
		 &( key_item->named_key ),
		 NULL );
	}
	return( -1 );
}

/* Reads a class name
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_read_class_name_data(
     libregf_key_item_t *key_item,
     const uint8_t *data,
     size_t data_size,
     uint16_t class_name_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_read_class_name_data";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid key item - class name value already set.",
		 function );

		return( -1 );
	}
	if( data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid data.",
		 function );

		return( -1 );
	}
	if( data_size > (size_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid data size value exceeds maximum.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: data:\n",
		 function );
		libcnotify_print_data(
		 data,
		 data_size,
		 0 );
	}
#endif
	if( ( class_name_size == 0 )
	 || ( (size_t) class_name_size > data_size ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid class name size value out of bounds.",
		 function );

		goto on_error;
	}
	key_item->class_name = (uint8_t *) memory_allocate(
	                                    sizeof( uint8_t ) * (size_t) class_name_size );

	if( key_item->class_name == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create class name.",
		 function );

		goto on_error;
	}
	if( memory_copy(
	     key_item->class_name,
	     data,
	     (size_t) class_name_size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to class name.",
		 function );

		goto on_error;
	}
	key_item->class_name_size = class_name_size;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( libregf_debug_print_utf16_string_value(
		     function,
		     "class name\t\t\t",
		     key_item->class_name,
		     (size_t) key_item->class_name_size,
		     LIBUNA_ENDIAN_LITTLE,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_PRINT_FAILED,
			 "%s: unable to print UTF-16 string value.",
			 function );

			goto on_error;
		}
		if( (size_t) class_name_size < data_size )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 &( data[ (size_t) class_name_size ] ),
			 data_size - (size_t) class_name_size,
			 0 );
		}
		else
		{
			libcnotify_printf(
			 "\n" );
		}
	}
#endif /* defined( HAVE_DEBUG_OUTPUT ) */

	return( 1 );

on_error:
	if( key_item->class_name != NULL )
	{
		memory_free(
		 key_item->class_name );

		key_item->class_name = NULL;
	}
	key_item->class_name_size = 0;

	return( -1 );
}

/* Reads a class name
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_read_class_name(
     libregf_key_item_t *key_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t class_name_offset,
     uint16_t class_name_size,
     libcerror_error_t **error )
{
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	static char *function                  = "libregf_key_item_read_class_name";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( key_item->class_name != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid key item - class name value already set.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( class_name_offset == 0xffffffffUL )
	{
		return( 1 );
	}
	if( ( class_name_offset == 0 )
	 && ( class_name_size == 0 ) )
	{
		return( 1 );
	}
	if( class_name_offset == 0 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid class name offset.",
		 function );

		return( -1 );
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: reading class name at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 class_name_offset,
		 class_name_offset );
	}
#endif
	if( libregf_hive_bins_list_get_cell_at_offset(
	     hive_bins_list,
	     file_io_handle,
	     class_name_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin at offset: %" PRIu32 " (0x%08" PRIx32 ").",
		 function,
		 class_name_offset,
		 class_name_offset );

		return( -1 );
	}
	if( libregf_key_item_read_class_name_data(
	     key_item,
	     hive_bin_cell->data,
	     hive_bin_cell->size,
	     class_name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read class name at offset: %" PRIu32 " (0x%08" PRIx32 ").",
		 function,
		 class_name_offset,
		 class_name_offset );

		return( -1 );
	}
	return( 1 );
}

/* Reads a security key
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_read_security_key(
     libregf_key_item_t *key_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t security_key_offset,
     libcerror_error_t **error )
{
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	libregf_security_key_t *security_key   = NULL;
	static char *function                  = "libregf_key_item_read_security_key";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( key_item->security_descriptor != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid key item - security descriptor value already set.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( ( security_key_offset == 0 )
	 || ( security_key_offset == 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid security key offset.",
		 function );

		return( -1 );
	}
	if( libregf_hive_bins_list_get_cell_at_offset(
	     hive_bins_list,
	     file_io_handle,
	     security_key_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin at offset: %" PRIu32 " (0x%08" PRIx32 ").",
		 function,
		 security_key_offset,
		 security_key_offset );

		goto on_error;
	}
	if( libregf_security_key_initialize(
	     &security_key,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create security key.",
		 function );

		goto on_error;
	}
	if( libregf_security_key_read_data(
	     security_key,
	     hive_bin_cell->data,
	     hive_bin_cell->size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read security key at offset: %" PRIu32 " (0x%08" PRIx32 ").",
		 function,
		 security_key_offset,
		 security_key_offset );

		goto on_error;
	}
	key_item->security_descriptor      = security_key->security_descriptor;
	key_item->security_descriptor_size = security_key->security_descriptor_size;

	security_key->security_descriptor      = NULL;
	security_key->security_descriptor_size = 0;

	if( libregf_security_key_free(
	     &security_key,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free security key.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( security_key != NULL )
	{
		libregf_security_key_free(
		 &security_key,
		 NULL );
	}
	return( -1 );
}

/* Reads a values list
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_read_values_list(
     libregf_key_item_t *key_item,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     uint32_t values_list_offset,
     uint32_t number_of_values_list_elements,
     libcerror_error_t **error )
{
	libregf_hive_bin_cell_t *hive_bin_cell = NULL;
	const uint8_t *hive_bin_cell_data      = NULL;
	static char *function                  = "libregf_key_item_read_values_list";
	size_t hive_bin_cell_size              = 0;
	uint32_t values_list_element_iterator  = 0;
	uint32_t values_list_element_offset    = 0;
	int element_index                      = 0;
	int hive_bin_index                     = 0;
	int result                             = 0;

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( number_of_values_list_elements == 0 )
	{
		return( 1 );
	}
	if( ( values_list_offset == 0 )
	 || ( values_list_offset == 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid values list offset.",
		 function );

		return( -1 );
	}
	if( libregf_hive_bins_list_get_cell_at_offset(
	     hive_bins_list,
	     file_io_handle,
	     values_list_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin at offset: %" PRIu32 " (0x%08" PRIx32 ").",
		 function,
		 values_list_offset ,
		 values_list_offset );

		return( -1 );
	}
	hive_bin_cell_data = hive_bin_cell->data;
	hive_bin_cell_size = hive_bin_cell->size;

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: data:\n",
		 function );
		libcnotify_print_data(
		 hive_bin_cell_data,
		 hive_bin_cell_size,
		 0 );
	}
#endif
	if( hive_bin_cell_size < ( number_of_values_list_elements * 4 ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid cell size value too small to contain number of values.",
		 function );

		return( -1 );
	}
	for( values_list_element_iterator = 0;
	     values_list_element_iterator < number_of_values_list_elements;
	     values_list_element_iterator++ )
	{
		byte_stream_copy_to_uint32_little_endian(
		 hive_bin_cell_data,
		 values_list_element_offset );

		hive_bin_cell_data += 4;
		hive_bin_cell_size -= 4;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: element: %03" PRIu32 " offset\t\t\t: 0x%08" PRIx32 "\n",
			 function,
			 values_list_element_iterator,
			 values_list_element_offset );
		}
#endif
		result = libregf_hive_bins_list_get_index_at_offset(
		          hive_bins_list,
		          (off64_t) values_list_element_offset,
		          &hive_bin_index,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine if values list element offset is valid.",
			 function );

			return( -1 );
		}
		else if( result == 0 )
		{
			key_item->item_flags |= LIBREGF_KEY_ITEM_FLAG_IS_CORRUPTED;
		}
		else if( libfdata_list_append_element(
		          key_item->values_list,
		          &element_index,
		          0,
		          (off64_t) values_list_element_offset,
		          0,
		          0,
		          error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_RESIZE_FAILED,
			 "%s: unable to set value list element: %" PRIu32 " in list.",
			 function,
			 values_list_element_iterator );

			return( -1 );
		}
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( hive_bin_cell_size > 0 )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 hive_bin_cell_data,
			 hive_bin_cell_size,
			 0 );
		}
		else
		{
			libcnotify_printf(
			 "\n" );
		}
	}
#endif
	return( 1 );
}

/* Reads a key
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_read_node_data(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     libfdata_tree_node_t *node,
     libfdata_cache_t *cache,
     int node_data_file_index LIBREGF_ATTRIBUTE_UNUSED,
     off64_t node_data_offset,
     size64_t node_data_size,
     uint32_t node_data_flags LIBREGF_ATTRIBUTE_UNUSED,
     uint8_t read_flags LIBREGF_ATTRIBUTE_UNUSED,
     libcerror_error_t **error )
{
	libregf_key_item_t *key_item = NULL;
	static char *function        = "libregf_key_item_read_node_data";

	LIBREGF_UNREFERENCED_PARAMETER( node_data_file_index )
	LIBREGF_UNREFERENCED_PARAMETER( node_data_flags )
	LIBREGF_UNREFERENCED_PARAMETER( read_flags )

	if( libregf_key_item_initialize(
	     &key_item,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create key item.",
		 function );

		goto on_error;
	}
	/* The size contains the hash of the name
	 */
	if( node_data_size > (size64_t) UINT32_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid node data size value exceeds maximum.",
		 function );

		goto on_error;
	}
	if( libregf_key_item_read_named_key(
	     key_item,
	     node,
	     file_io_handle,
	     hive_bins_list,
	     node_data_offset,
	     (uint32_t) node_data_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read named key at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 node_data_offset,
		 node_data_offset );

		goto on_error;
	}
	if( libfdata_tree_node_set_node_value(
	     node,
	     cache,
	     (intptr_t *) key_item,
	     (int (*)(intptr_t **, libcerror_error_t **)) &libregf_key_item_free,
	     LIBFDATA_TREE_NODE_VALUE_FLAG_MANAGED,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set key item as node value.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( key_item != NULL )
	{
		libregf_key_item_free(
		 &key_item,
		 NULL );
	}
	return( -1 );
}

/* Reads a sub keys list
 * Returns 1 if successful, 0 if not or -1 on error
 */
int libregf_key_item_read_sub_keys_list(
     libfdata_tree_node_t *key_tree_node,
     libbfio_handle_t *file_io_handle,
     libregf_hive_bins_list_t *hive_bins_list,
     off64_t sub_keys_list_offset,
     libcerror_error_t **error )
{
	libregf_hive_bin_cell_t *hive_bin_cell    = NULL;
	const uint8_t *hive_bin_cell_data         = NULL;
	const uint8_t *sub_keys_list_data         = NULL;
	static char *function                     = "libregf_key_item_read_sub_keys_list";
	size_t hive_bin_cell_size                 = 0;
	uint32_t element_hash                     = 0;
	uint32_t sub_keys_list_element_offset     = 0;
	uint16_t number_of_sub_keys_list_elements = 0;
	uint16_t sub_keys_list_element_iterator   = 0;
	uint8_t sub_keys_list_element_size        = 0;
	uint8_t at_leaf_level                     = 0;
	int corruption_detected                   = 0;
	int hive_bin_index                        = 0;
	int result                                = 0;
	int sub_node_index                        = 0;

	if( hive_bins_list == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid hive bins list.",
		 function );

		return( -1 );
	}
	if( ( sub_keys_list_offset == 0 )
	 || ( sub_keys_list_offset == (off64_t) 0xffffffffUL ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: invalid sub keys list offset.",
		 function );

		return( -1 );
	}
	if( libregf_hive_bins_list_get_cell_at_offset(
	     hive_bins_list,
	     file_io_handle,
	     (uint32_t) sub_keys_list_offset,
	     &hive_bin_cell,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve hive bin at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 sub_keys_list_offset,
		 sub_keys_list_offset );

		goto on_error;
	}
#if SIZEOF_SIZE_T <= 4
	if( hive_bin_cell->size > (uint32_t) SSIZE_MAX )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid hive bin cell - size value exceeds maximum.",
		 function );

		return( -1 );
	}
#endif
	/* Make a local copy of the data in case the hive bin cell gets cached out
	 * while reading the sub keys
	 */
	hive_bin_cell_data = memory_allocate(
	                      sizeof( uint8_t ) * hive_bin_cell->size );

	if( hive_bin_cell_data == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create hive bin cell data.",
		 function );

		goto on_error;
	}
	hive_bin_cell_size = hive_bin_cell->size;

	if( memory_copy(
	     hive_bin_cell_data,
	     hive_bin_cell->data,
	     hive_bin_cell->size ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_COPY_FAILED,
		 "%s: unable to copy hive bin cell data.",
		 function );

		goto on_error;
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: data:\n",
		 function );
		libcnotify_print_data(
		 hive_bin_cell_data,
		 hive_bin_cell_size,
		 0 );
	}
#endif
	if( hive_bin_cell_size < sizeof( regf_sub_key_list_t ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid hive bin cell size too small.",
		 function );

		goto on_error;
	}
	/* Check if the cell signature matches that of a sub keys list: "lf", "lh", "li" or "ri"
	 */
	if( ( hive_bin_cell_data[ 0 ] == (uint8_t) 'r' )
	 && ( hive_bin_cell_data[ 1 ] == (uint8_t) 'i' ) )
	{
		sub_keys_list_element_size = 4;
		at_leaf_level              = 0;
	}
	else if( ( hive_bin_cell_data[ 0 ] == (uint8_t) 'l' )
	      && ( hive_bin_cell_data[ 1 ] == (uint8_t) 'i' ) )
	{
		sub_keys_list_element_size = 4;
		at_leaf_level              = 1;
	}
	else if( ( hive_bin_cell_data[ 0 ] == (uint8_t) 'l' )
	      && ( ( hive_bin_cell_data[ 1 ] == (uint8_t) 'f' )
	        || ( hive_bin_cell_data[ 1 ] == (uint8_t) 'h' ) ) )
	{
		sub_keys_list_element_size = 8;
		at_leaf_level              = 1;
	}
	else
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
		 "%s: unsupported sub keys list signature.",
		 function );

		goto on_error;
	}
	byte_stream_copy_to_uint16_little_endian(
	 ( (regf_sub_key_list_t *) hive_bin_cell_data )->number_of_elements,
	 number_of_sub_keys_list_elements );

#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		libcnotify_printf(
		 "%s: signature\t\t\t\t: %c%c\n",
		 function,
		 ( (regf_sub_key_list_t *) hive_bin_cell_data )->signature[ 0 ],
		 ( (regf_sub_key_list_t *) hive_bin_cell_data )->signature[ 1 ] );

		libcnotify_printf(
		 "%s: number of elements\t\t\t: %" PRIu16 "\n",
		 function,
		 number_of_sub_keys_list_elements );
	}
#endif
	sub_keys_list_data  = &( hive_bin_cell_data[ sizeof( regf_sub_key_list_t ) ] );
	hive_bin_cell_size -= sizeof( regf_sub_key_list_t );

	if( hive_bin_cell_size < (size_t) ( number_of_sub_keys_list_elements * sub_keys_list_element_size ) )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: invalid cell size value too small to contain number of elements.",
		 function );

		goto on_error;
	}
	for( sub_keys_list_element_iterator = 0;
	     sub_keys_list_element_iterator < number_of_sub_keys_list_elements;
	     sub_keys_list_element_iterator++ )
	{
		byte_stream_copy_to_uint32_little_endian(
		 sub_keys_list_data,
		 sub_keys_list_element_offset );

		sub_keys_list_data += 4;
		hive_bin_cell_size -= 4;

#if defined( HAVE_DEBUG_OUTPUT )
		if( libcnotify_verbose != 0 )
		{
			libcnotify_printf(
			 "%s: element: %03" PRIu16 " offset\t\t: 0x%08" PRIx32 "\n",
			 function,
			 sub_keys_list_element_iterator,
			 sub_keys_list_element_offset );
		}
#endif
		if( sub_keys_list_element_size == 8 )
		{
			byte_stream_copy_to_uint32_little_endian(
			 sub_keys_list_data,
			 element_hash );

			sub_keys_list_data += 4;
			hive_bin_cell_size -= 4;

#if defined( HAVE_DEBUG_OUTPUT )
			if( libcnotify_verbose != 0 )
			{
				libcnotify_printf(
				 "%s: element: %03" PRIu16 " hash\t\t\t: 0x%08" PRIx32 "\n",
				 function,
				 sub_keys_list_element_iterator,
				 element_hash );
			}
#endif
		}
		else
		{
			element_hash = 0;
		}
		result = libregf_hive_bins_list_get_index_at_offset(
		          hive_bins_list,
		          (off64_t) sub_keys_list_element_offset,
		          &hive_bin_index,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to determine if sub keys list element offset is valid.",
			 function );

			goto on_error;
		}
		else if( result != 0 )
		{
			if( at_leaf_level != 0 )
			{
				if( libfdata_tree_node_append_sub_node(
				     key_tree_node,
				     &sub_node_index,
				     0,
				     (off64_t) sub_keys_list_element_offset,
				     (size64_t) element_hash,
				     0,
				     error ) != 1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_RUNTIME,
					 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
					 "%s: unable to append sub node.",
					 function );

					goto on_error;
				}
			}
			else
			{
				result = libregf_key_item_read_sub_keys_list(
					  key_tree_node,
					  file_io_handle,
					  hive_bins_list,
					  (off64_t) sub_keys_list_element_offset,
					  error );

				if( result == -1 )
				{
					libcerror_error_set(
					 error,
					 LIBCERROR_ERROR_DOMAIN_IO,
					 LIBCERROR_IO_ERROR_READ_FAILED,
					 "%s: unable to read sub keys list at offset: %" PRIu32 " (0x%08" PRIx32 ").",
					 function,
					 sub_keys_list_element_offset,
					 sub_keys_list_element_offset );

					goto on_error;
				}
			}
		}
		if( result == 0 )
		{
			corruption_detected = 1;
		}
	}
#if defined( HAVE_DEBUG_OUTPUT )
	if( libcnotify_verbose != 0 )
	{
		if( hive_bin_cell_size > 0 )
		{
			libcnotify_printf(
			 "%s: padding:\n",
			 function );
			libcnotify_print_data(
			 sub_keys_list_data,
			 hive_bin_cell_size,
			 0 );
		}
		else
		{
			libcnotify_printf(
			 "\n" );
		}
	}
#endif
	memory_free(
	 hive_bin_cell_data );

	if( corruption_detected != 0 )
	{
		return( 0 );
	}
	return( 1 );

on_error:
	if( hive_bin_cell_data != NULL )
	{
		memory_free(
		 hive_bin_cell_data );
	}
	return( -1 );
}

/* Reads the sub keys
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_read_sub_nodes(
     libregf_hive_bins_list_t *hive_bins_list,
     libbfio_handle_t *file_io_handle,
     libfdata_tree_node_t *node,
     libfdata_cache_t *cache LIBREGF_ATTRIBUTE_UNUSED,
     int sub_nodes_data_file_index LIBREGF_ATTRIBUTE_UNUSED,
     off64_t sub_nodes_data_offset,
     size64_t sub_nodes_data_size LIBREGF_ATTRIBUTE_UNUSED,
     uint32_t sub_nodes_data_flags LIBREGF_ATTRIBUTE_UNUSED,
     uint8_t read_flags LIBREGF_ATTRIBUTE_UNUSED,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_read_sub_nodes";
	int result            = 0;

	LIBREGF_UNREFERENCED_PARAMETER( cache )
	LIBREGF_UNREFERENCED_PARAMETER( sub_nodes_data_file_index )
	LIBREGF_UNREFERENCED_PARAMETER( sub_nodes_data_size )
	LIBREGF_UNREFERENCED_PARAMETER( sub_nodes_data_flags )
	LIBREGF_UNREFERENCED_PARAMETER( read_flags )

	result = libregf_key_item_read_sub_keys_list(
	          node,
	          file_io_handle,
	          hive_bins_list,
	          sub_nodes_data_offset,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_IO,
		 LIBCERROR_IO_ERROR_READ_FAILED,
		 "%s: unable to read sub keys list at offset: %" PRIi64 " (0x%08" PRIx64 ").",
		 function,
		 sub_nodes_data_offset,
		 sub_nodes_data_offset );

		return( -1 );
	}
	else if( result == 0 )
	{
/* TODO signal corruption */
	}
	return( 1 );
}

/* Retrieves the number of key item
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_number_of_values(
     libregf_key_item_t *key_item,
     int *number_of_values,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_number_of_values";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libfdata_list_get_number_of_elements(
	     key_item->values_list,
	     number_of_values,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of elements from values data list.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the key name size
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_name_size(
     libregf_key_item_t *key_item,
     size_t *name_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_name_size";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_name_size(
	     key_item->named_key,
	     name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve name size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the key name
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_name(
     libregf_key_item_t *key_item,
     uint8_t *name,
     size_t name_size,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_name";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_name(
	     key_item->named_key,
	     name,
	     name_size,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string size of the key name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_utf8_name_size(
     libregf_key_item_t *key_item,
     size_t *utf8_string_size,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_utf8_name_size";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_utf8_name_size(
	     key_item->named_key,
	     utf8_string_size,
	     ascii_codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-8 name size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-8 string value of the key name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_utf8_name(
     libregf_key_item_t *key_item,
     uint8_t *utf8_string,
     size_t utf8_string_size,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_utf8_name";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_utf8_name(
	     key_item->named_key,
	     utf8_string,
	     utf8_string_size,
	     ascii_codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-8 name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 string size of the key name
 * The returned size includes the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_utf16_name_size(
     libregf_key_item_t *key_item,
     size_t *utf16_string_size,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_utf16_name_size";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_utf16_name_size(
	     key_item->named_key,
	     utf16_string_size,
	     ascii_codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-16 name size.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Retrieves the UTF-16 string value of the key name
 * The function uses a codepage if necessary, it uses the codepage set for the library
 * The size should include the end of string character
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_utf16_name(
     libregf_key_item_t *key_item,
     uint16_t *utf16_string,
     size_t utf16_string_size,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_utf16_name";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_utf16_name(
	     key_item->named_key,
	     utf16_string,
	     utf16_string_size,
	     ascii_codepage,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve UTF-16 name.",
		 function );

		return( -1 );
	}
	return( 1 );
}

/* Compares the key name with UTF-8 string
 * Returns 1 if the names match, 0 if not or -1 on error
 */
int libregf_key_item_compare_name_with_utf8_string(
     libregf_key_item_t *key_item,
     uint32_t name_hash,
     const uint8_t *utf8_string,
     size_t utf8_string_length,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_compare_name_with_utf8_string";
	int result            = 0;

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	result = libregf_named_key_compare_name_with_utf8_string(
	          key_item->named_key,
	          name_hash,
	          utf8_string,
	          utf8_string_length,
	          ascii_codepage,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GENERIC,
		 "%s: unable to compare sub key name with UTF-8 string.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Compares the key name with UTF-16 string
 * Returns 1 if the names match, 0 if not or -1 on error
 */
int libregf_key_item_compare_name_with_utf16_string(
     libregf_key_item_t *key_item,
     uint32_t name_hash,
     const uint16_t *utf16_string,
     size_t utf16_string_length,
     int ascii_codepage,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_compare_name_with_utf16_string";
	int result            = 0;

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	result = libregf_named_key_compare_name_with_utf16_string(
	          key_item->named_key,
	          name_hash,
	          utf16_string,
	          utf16_string_length,
	          ascii_codepage,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GENERIC,
		 "%s: unable to compare sub key name with UTF-16 string.",
		 function );

		return( -1 );
	}
	return( result );
}

/* Retrieves the 64-bit FILETIME value of the last written date and time
 * Returns 1 if successful or -1 on error
 */
int libregf_key_item_get_last_written_time(
     libregf_key_item_t *key_item,
     uint64_t *filetime,
     libcerror_error_t **error )
{
	static char *function = "libregf_key_item_get_last_written_time";

	if( key_item == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid key item.",
		 function );

		return( -1 );
	}
	if( libregf_named_key_get_last_written_time(
	     key_item->named_key,
	     filetime,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve last written time.",
		 function );

		return( -1 );
	}
	return( 1 );
}

