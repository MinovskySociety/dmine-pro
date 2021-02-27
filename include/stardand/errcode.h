#pragma once

#ifndef __ERRORCODE
#define __ERRORCODE

#define E_SUCCEEDED(r)							(((int32_t)(r)) >= 0)
#define E_FAILED(r)								(((int32_t)(r)) < 0)

//ERROR CODE
#define E_OK														0
#define E_ERROR_UNKNOWN												-1
#define E_ERROR_NOT_SUPPORTED										-2
#define E_ERROR_LIBRARY_NOT_ININIALIZED								-3
#define E_ERROR_NOT_ENOUGH_MEMORY									-4
#define E_ERROR_TIMEOUT												-5
#define E_ERROR_INVALID_PARAMETER									-6
#define E_ERROR_INVALID_DATA										-7
#define E_ERROR_INVALID_OUTPUT_POINTER								-8
#define E_ERROR_BUFFER_TOO_SMALL									-9
#define E_ERROR_NOT_FOUND											-10
#define E_ERROR_CLOSED												-11
#define E_ERROR_ALREADY_OPENED										-12
#define E_ERROR_WORKING												-13
#define E_ERROR_INVALID_PARAMETER_COMBINATION						-14
#define E_ERROR_CANCELED											-15
#define E_ERROR_NAME_EXISTS											-16
#define E_ERROR_GROUP_NOT_FOUND										-17
#define E_ERROR_TEMPLATE_NOT_FOUND									-18
#define E_ERROR_MAX_NUM_REACHED										-19

//VF2
#define E_ERROR_NOT_FIND_MATCH                                      -2001
#define E_ERROR_NOT_SATISFY_RULES                                   -2002 
#define E_ERROR_NOT_JOINABLE                                        -2003
#define E_ERROR_NOT_INCANDIDATE_SET                                 -2004
//SubgraphIsomorphism
#define E_ERROR_NOT_FIND_ISO                                        -3001


#define E_ERROR_ALREADY_LOCKED										E_ERROR_WORKING
#define FVC_OK														E_OK
#define E_ERROR_INVALID_FVC_HANDLE									-10015
#define E_ERROR_FAIL_PACK_YNLCPROT									-10016

#define E_ERROR_NOT_AVAILABLE										-10017

#define IOSTREAM_OK												E_OK
#define IOSTREAM_FAIL											-1

#endif//#ifndef __EWX1000SDK
