/****************************************************************
 *                  
 *   HEADER FILE:  MP.h                 
 *        
 *        Everything we need.  Saves doing a zillion includes
 *        in user programs.
 *
 ***************************************************************/

#ifndef _MP_h
#define _MP_h

/*
 * Definitions that allow this header file to be used either with or
 * without ANSI C features like function prototypes.
 */

#undef _ANSI_ARGS_
#undef CONST
#if ((defined(__STDC__) || defined(SABER))  && !defined(NO_PROTOTYPE)) || defined(__cplusplus)
#   define _ANSIC_
#   define _USING_PROTOTYPES_ 1
#   define _ANSI_ARGS_(x)       x
#   define CONST const
#   ifdef __cplusplus
#       define VARARGS (...)
#   else
#       define VARARGS ()
#   endif
#else
#   define _ANSI_ARGS_(x)      ()
#   define CONST
#endif

#ifdef __cplusplus
#   define EXTERN extern "C"
#else
#   define EXTERN extern
#endif


/* standard includes */

#include <stddef.h>
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/* MP Config include */
#include "MP_Config.h"

/* need select.h where available */
#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif


/* MP includes */


/* Stuff included from h/MP_Types.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_Types.h                                         
 *                                                                
 *        Most of the typedefs needed by MP are here, including the definition 
 *      of node and annotation packets.                  
 *        
 *   Change Log:
 *     May 30, 1995 SG - added defines for new types for MP 1.0        
 *     December 12, 1995 SG - added ifdef for AIX per Hannes' instructions
 *     December 20, 1995 SG - added enum for fp format
 *     4/23/96 sgray - added defines to support different arbitrary precision
 *                     integer formats.
 *     Jan 6, 1997  OB - rearranged integer assignments for MP types
 *
 ***************************************************************************/

#ifndef _MP_Types_h
#define _MP_Types_h

#define MP_TRUE  0x01
#define MP_FALSE 0x00

#ifdef __hpux
#   define SELECT(maxfdp1, readfs, writefs, exceptfds, tvptr) \
        select((int)maxfdp1, (int*)readfs, (int*)writefs,     \
               (int*)exceptfds, tvptr)
#else /*  not __hpux */
#   define SELECT  select
#endif /* not __hpux */


/* to indicate the direction of traffic */
#define MP_SEND     0
#define MP_RECEIVE  1

/* For checking the status of a link*/
enum link_status_option {MP_LinkReadyReading, MP_LinkReadyWriting,
                         MP_LinkIsParent};
typedef enum link_status_option MP_LinkStatus_t;

/* To determine which byte ordering is native and which is actually used. */
/* The default is BigEndian. */
enum word_order {MP_BigEndian, MP_LittleEndian};
typedef enum word_order MP_WordOrder_t;

/* To determine which floating format is native and which is actually used. */
/* The default is IEEE format. */
enum fp_format {MP_IEEE_fp, MP_Vax_fp};
typedef enum fp_format MP_FpFormat_t;



/*
*  Important size used throughout
*/
#define  MP_BytesPerMPUnit        4

/* New defines for MP 1.0 */

/* shift values for the node packet */
#define  MP_NodeOpFieldShift           8
#define  MP_NodeNumChildFieldShift     8
#define  MP_NodeAnnotFieldShift       12
#define  MP_NodeDictTagFieldShift     16
#define  MP_NodeCommonValueFieldShift 24

/* used for the #children and #annots field in the node packet header
   to indicate that there are more than 14 children or annotations and
   that an extra field(s) (a long) must be retrieved from the stream */
#define  MP_ExtraFieldIndicator             0xf

/* used for the dictionary tag field to indicate that an extra four
   bytes must be read to get the dictionary tag - don't expect to
   actually have to use this for a long time                         */
#define  MP_ExtraMPDictTagFieldIndicator    0x7f
#define  MP_ExtraUserDictTagFieldIndicator  0xff

/* masks for the node packet fields */
#define  MP_NodeTypeMask         0x000000ff
#define  MP_NodeNumChildMask     0x00000f00
#define  MP_NodeNumAnnotMask     0x0000f000
#define  MP_NodeDictTagMask      0x00ff0000
#define  MP_NodeCommonValueMask  0xff000000


#define MP_ExtraAnnotField   MP_ExtraFieldIndicator << MP_NodeAnnotFieldShift
#define MP_ExtraChildField   MP_ExtraFieldIndicator << MP_NodeOpFieldShift
#define MP_ExtraDictTagField MP_ExtraFieldIndicator << MP_NodeDictTagFieldShift


/*
*  The format of a node packet header is:
*
*  |    3           2            1              0    |
*  +-----------+-----------|-------------+-----------+
*  |  common   |dictionary |  ^   |  ^   |    Node   |
*  |  value    |   tag     |  |   |  |   |    type   |
*                             |      |- #children
*                             |- #annots
*/

typedef unsigned char  MP_NodeType_t;
typedef unsigned char  MP_CommonValue_t;
typedef unsigned long  MP_NodeHeader_t;
typedef unsigned long  MP_NumChild_t;
typedef unsigned long  MP_NumAnnot_t;
typedef unsigned long  MP_DictTag_t;      /* common to annotation packet header */

/* macros for setting and getting the fields of a node header */

#define MP_SetNodeTypeField(hdr, type)              \
     (hdr = (hdr & ~MP_NodeTypeMask) | (long) type)

#define MP_SetNodeNumChildField(hdr, nchild)                 \
     (hdr =  (hdr & ~MP_NodeNumChildMask)                    \
             | ((long) nchild << MP_NodeNumChildFieldShift))

#define MP_SetNodeNumAnnotsField(hdr, nannots)            \
     (hdr = (hdr & ~MP_NodeNumAnnotMask)                  \
            | ((long) nannots << MP_NodeAnnotFieldShift))

#define MP_SetNodeDictTagField(hdr, dtag)         \
     (hdr = (hdr & ~MP_NodeDictTagMask)           \
            | (dtag << MP_NodeDictTagFieldShift))

#define MP_SetNodeExtraChildField(hdr)  \
     (hdr |= (long) MP_ExtraChildField)

#define MP_SetNodeExtraAnnotsField(hdr)  \
     (hdr |= (long) MP_ExtraAnnotField)

#define MP_SetNodeCommonValueField(hdr, value)                 \
     (hdr = (hdr & ~MP_NodeCommonValueMask)                    \
            | ((long) value <<  MP_NodeCommonValueFieldShift))

#define MP_GetNodeFieldType(hdr) \
     (hdr & MP_NodeTypeMask)

#define MP_GetNodeNumAnnotsField(hdr) \
     ((hdr & MP_NodeNumAnnotMask) >> MP_NodeAnnotFieldShift)

#define MP_GetNodeNumChildField(hdr) \
     ((hdr & MP_NodeNumChildMask) >> MP_NodeOpFieldShift)

#define MP_GetNodeDictTagField(hdr) \
     ((hdr & MP_NodeDictTagMask) >> MP_NodeDictTagFieldShift)

#define MP_GetNodeCommonValueField(hdr) \
    ((hdr & MP_NodeCommonValueMask) >> MP_NodeCommonValueFieldShift)

/* see MP_Annotations.h for the defines for annotations */

/* Note:  plain cc (old style?) doesn't like signed char!! */
typedef signed char      MP_Sint8_t;
typedef unsigned char    MP_Uint8_t;
typedef unsigned short   MP_Uint16_t;   /* aren't real MP types yet */
typedef signed short     MP_Sint16_t;
typedef long             MP_Sint32_t;
typedef unsigned long    MP_Uint32_t;
typedef float            MP_Real32_t;
typedef double           MP_Real64_t;
typedef unsigned char    MP_Boolean_t;
typedef unsigned char    MP_Common_t;  /* for common types rep as a char */
typedef void *           MP_ApInt_t;
typedef MP_ApInt_t *   MP_ApInt_pt;
typedef void *          MP_ApReal_t;
typedef MP_ApReal_t *   MP_ApReal_pt;

typedef struct MP_Link_t MP_Link_t;
typedef MP_Link_t * MP_Link_pt;

/*
   encodings for the Multi dictionaries
*/
#define MP_BasicDict              0
#define MP_NoSemanticsDict      128
#define MP_ReceiverDict         129
#define MP_UnavailSemanticsDict 130
#define MP_LocalDict            155


/*
*  encodings for the Multi types plus some useful macros
*/
#define MP_Sint8Type                        65
#define MP_Uint8Type                        67
#define MP_BooleanType                      68
#define MP_CommonLatinIdentifierType        72
#define MP_CommonGreekIdentifierType        76
#define MP_CommonConstantType               80
#define MP_CommonMetaType                   84
#define MP_CommonMetaOperatorType           88

#define MP_StringType                       36
#define MP_ConstantType                     40
#define MP_IdentifierType                   44
#define MP_MetaType                         48
#define MP_MetaOperatorType                 52

#define MP_Sint32Type                       17
#define MP_Uint32Type                       19
#define MP_Real32Type                       18

#define MP_ApIntType                        9
#define MP_ApRealType                       10

#define  MP_Real64Type                      6

#define MP_OperatorType                     128
#define MP_CommonOperatorType               129

#define MP_RawType                          134

/* MP_ANY_TYPE is used by the internal get node packet routine */
#define MP_ANY_TYPE                     255


/* And here are some Handy Macros */
/* Basic types, i.e. those which appear as a leaf of a data tree
   have numerical values between 0 and 127 ,i.e. bit 8 is zero */
#define MP_IsBasicType(type)    (!((type) & 128))

/* native 8-bit types have numeric values between 64 and 127
   i.e. bit 8 is zero and bit 7 is one */
#define MP_Is8BitNumericType(type)    (!((type) & 128) && ((type) & 64))

/* native String-based types have numeric values between 32 an 63,
   i.e. bit 8,7 are zero, bit 6 is one */
#define MP_IsStringBasedType(type)  (!((type) & 192) && ((type) & 32))

/* native 32 bit types have numeric values beween 16 and 31, i.e. bits
   8,7,6, are zero, bit 5 is one. Furthermore, integers have bit 1 set
   to 1 and Reals have bit 1 set to 0 */
#define MP_Is32BitNumericType(type)    (!((type) & 224) && ((type) & 16))


/* Ap Numbers have bit 5-8 set to zero, bit 4 set to one */
#define MP_IsApNumberType(type) (!((type) & 240) && ((type) & 8))

/* Native 64-bit numbers have bit 4-8 set to zero, bit 3 set to one */
#define MP_Is64BitNumericType(type)    (type == MP_Real64Type)

/* Predicates for real and integers
   Integers are odd numbers, reals are even numbers, not divisble by 4*/
#define MP_IsRealType(type)   (MP_IsBasicType(type) &&    \
                                 !((type) & 1) &&           \
                                 ((type) >> 1) & 1)
#define MP_IsIntegerType(type)(MP_IsBasicType(type) && ((type) & 1))

#define MP_IsFixedIntegerType(type)                     \
(MP_IsIntegerType(type) && ! (type == MP_ApIntType))

#define MP_IsIdType(type)                       \
(type  == MP_IdentifierType ||                  \
 type == MP_CommonLatinIdentifierType ||        \
 type == MP_CommonGreekIdentifierType)

#define MP_IsCommonType(type) ((type >= 72 && type <= 88) || type == 129)
#endif /* _MP_Types_h */

/* Stuff included from h/MP_Annotations.h */
/****************************************************************
 *
 *   HEADER FILE:  MP_Annotations.h
 *
 *        Define the MP-defined annotations and establish
 *        macros to set and clear the annotation packet
 *        flag fields.
 *
 *   Change Log:
 *     5/9/96  sgray - Fixed macros to set/clear the annotation
 *                     packet header flags field.
 *                   - Added predicate macros for the flags field
 *                     per suggestion from obachman (good friend
 *                     and drinking buddy).
 ****************************************************************/

#ifndef _MP_Annotations_h
#define _MP_Annotations_h

/* global variables - maps annotation numbers to string
 * identifying the annotation - used for testing.  See 
 * the corresponding dictionary for the defines.
 */
EXTERN char *MpAnnotations[];         /* MP_MpDict.h    */
EXTERN char *ProtoAnnotations[];      /* MP_ProtoDict.h */

/*
 * Annotation structure
 *  The format of a annotation packet  is:
 *
 *  |     3           2            1            0     |
 *  +-----------+-----------|-------------+-----------+
 *  |   flags   |dictionary |        Annot type       |
 *  |           |   tag     |                         |
 *
 *  sizes, typedefs, and masks for the fields of an
 *  annotation packet
 */

typedef  unsigned char  MP_AnnotFlags_t;
typedef  unsigned short MP_AnnotType_t;
typedef  unsigned long  MP_AnnotHeader_t;

#define  MP_AnnotTypeMask    0x0000ffff
#define  MP_AnnotDictTagMask 0x00ff0000
#define  MP_AnnotFlagsMask   0xff000000

/*  Format of an annotation packet by field */

#define  MP_AnnotDictTagFieldShift     16
#define  MP_AnnotFlagsFieldShift       24


/*
 * Annotation flags
 *                                         0             1
 */
#define MP_AnnotRequired    0x0001  /* supplemental   required   */
#define MP_AnnotValuated    0x0002  /* simple         valuated   */
#define MP_AnnotTreeScope   0x0004  /* node           subtree    */


/*
 * macros to set and unset flag fields
 */
#define MP_SetAnnotRequired(aflags)    (aflags |= MP_AnnotRequired)
#define MP_SetAnnotValuated(aflags)    (aflags |= MP_AnnotValuated)
#define MP_SetAnnotTreeScope(aflags)   (aflags |= MP_AnnotTreeScope)
#define MP_ClearAnnotRequired(aflags)  (aflags &= ~MP_AnnotRequired)
#define MP_ClearAnnotValuated(aflags)  (aflags &= ~MP_AnnotValuated)
#define MP_ClearAnnotTreeScope(aflags) (aflags &= ~MP_AnnotTreeScope)
#define MP_ClearAnnotFlags(aflags)     (aflags &= 0x0000)

/*
 * macros to detrmine if an annotation flag is set
 */
#define MP_IsAnnotRequired(aflags)     (aflags & MP_AnnotRequired)
#define MP_IsAnnotValuated(aflags)     (aflags & MP_AnnotValuated)
#define MP_IsAnnotTreeScope(aflags)    (aflags & MP_AnnotTreeScope)

/*
 * macros to set the fields of an annotation packet header
 */
#define MP_SetAnnotTypeField(hdr, type) \
     (hdr = (hdr & ~MP_AnnotTypeMask) | (long) type)

#define MP_SetAnnotDictTagField(hdr, dtag) \
     (hdr = (hdr & ~MP_AnnotDictTagMask) | (dtag << MP_AnnotDictTagFieldShift))

#define MP_SetAnnotFlagsField(hdr, aflag)          \
     (hdr = (hdr & ~MP_AnnotFlagsMask)             \
     | ((long) aflag <<  MP_AnnotFlagsFieldShift))

#define MP_GetAnnotFieldType(hdr) \
     (hdr & MP_AnnotTypeMask)

#define MP_GetAnnotDictTagField(hdr) \
     ((hdr & MP_AnnotDictTagMask) >> MP_AnnotDictTagFieldShift)

#define MP_GetAnnotFlagsField(hdr) \
    ((hdr & MP_AnnotFlagsMask) >> MP_AnnotFlagsFieldShift)


#endif /* _MP_Annotations_h */

/* Stuff included from h/MP_Error.h */
/***************************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Error.h 
 * 
 *        All the error encodings and the routines the set clear
 *        and print errors.
 *                                                                
 *  Change Log:
 *       10/18/95 OB      - put MP_Failure, MP_Success into enum MP_Errors
 *       July 7, 1995  SG - updated MP_Errors to reflect newtypes
 *                          in MP-1.0 and coelesced a few error types.
 *                          Also made the necessary changes to
 *                          MP_Error.h, MP_Put.c, MP_Get.c
 *       July 21, 1995 SG - more additions/corrections.  Checked against
 *                          MP_Error.c for accuracy (enums with corresponding
 *                          error messages)
 *
 *       5/24/96 sgray - deleted a bunch of obsolete error codes (consolidated)
 *
 ***************************************************************************/

#ifndef _MP_Error_h
#define _MP_Error_h

#include "MP.h"
/*
 *  for returning error status from most routines
 */

enum MP_Errors { MP_Failure,
                 MP_Success,
                 MP_UnknownType,
                 MP_UnknownAnnotation,
                 MP_UnknownRequiredAnnotation,
                 MP_NullLink,
                 MP_NullTransport,
                 MP_CantOpenFile,
                 MP_CantConnect,
                 MP_MemAlloc,
                 MP_CantConvert,
                 MP_ValueOutOfRange,
                 MP_CantPutNodeHeader,
                 MP_CantPutNodePacket,
                 MP_CantPutDataPacket,
                 MP_CantPutAnnotationPacket,
                 MP_CantGetNodeHeader,
                 MP_CantGetNodePacket,
                 MP_CantGetDataPacket,
                 MP_CantGetAnnotationPacket,
                 MP_NotRequestedPacketType,
                 MP_CantConvertBadString,
                 MP_CantWriteLink,
                 MP_CantReadLink,
                 MP_NotCommonIdentifier,
		 MP_WrongBasicVectorType,
                 MP_DummyBigNum,
                 MP_MaxError };

typedef enum MP_Errors  MP_Status_t;

EXTERN char *MP_errlist[];


#define CHK_ALLOC_MEM(link, ptr, len, ptr_type) {                \
  if (ptr == NULL)                                               \
    if ((ptr = IMP_MemAllocFnc(len * sizeof(ptr_type))) == NULL) \
      return MP_SetError(link, MP_MemAlloc);                     \
}

#define TEST_MEM_ALLOC_ERROR(link, ptr) {    \
   if (ptr == NULL)                          \
      return MP_SetError(link, MP_MemAlloc); \
}

#define RETURN_OK(link)       \
   return MP_ClearError(link)

#define ERR_CHK(fnc_status) {    \
   if (fnc_status != MP_Success) \
      return MP_Failure;         \
}

#endif  /* _MP_Error_h */

/* Stuff included from h/MP_BuffTypes.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_BuffTypes.h
 *
 *       Contains all declarations related to buffer and buffer pool.
 *
 ****************************************************************/
#ifndef _MP_BuffTypes_h
#define _MP_BuffTypes_h

 
#define MP_MINIMUM_BUFF_SIZE       512
#define MP_DEFAULT_BUFF_SIZE      1024
#define MP_DEFAULT_MAX_FREE_BUFF    32
#define MP_DEFAULT_INIT_FREE_BUFF   16

struct buffer_handle {
        char * the_buffer; /* ptr to block of memory */
        char * buff;       /* byte aligned ptr to buffer */
        struct buffer_handle *next;
};

typedef struct buffer_handle buffer_handle_t;
typedef buffer_handle_t  *buffer_handle_pt;

struct buffer_pool {
        int      buff_size;           /* buffer size */
        int      max_free_buff;       /* max. no. of free buff. */
        int      curr_free_buff;      /* current no. of free buff. */ 
        buffer_handle_pt   buff;      /* ptr to list of buff handle */
};

typedef struct buffer_pool buffer_pool_t;
typedef buffer_pool_t *buffer_pool_pt;

#endif

/* Stuff included from h/MP_Memory.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Memory.h
 *                                                                
 ***************************************************************/


#ifndef _MP_Memory_h
#define _MP_Memory_h
 
EXTERN void* (*IMP_MemAllocFnc) _ANSI_ARGS_((size_t));
EXTERN void* (*IMP_MemReallocFnc) _ANSI_ARGS_((void *, size_t, size_t));
EXTERN void  (*IMP_MemFreeFnc) _ANSI_ARGS_((void *, size_t));

EXTERN void* (*IMP_RawMemAllocFnc) _ANSI_ARGS_((size_t));
EXTERN void* (*IMP_RawMemReallocFnc) _ANSI_ARGS_((void *, size_t));
EXTERN void  (*IMP_RawMemFreeFnc) _ANSI_ARGS_((void *));


#endif /* _MP_Memory_h */




/* Stuff included from h/MP_BigNum.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_BigNum.h
 *                        
 * The relevant declarations for the handling of BigNums
 *
 ****************************************************************/

#ifndef _MP_BigNum_h
#define _MP_BigNum_h

#include "MP.h"

/* Note: a type_to_string conversion array using the ordering given */
/* below is defined in MP_Env.c.  So if you change the order below, */
/* make the corresponding change in MP_Env.c, okay?                 */

typedef MP_Uint8_t MP_BigNumFormat_t;

typedef struct MP_BigIntOps_t
{
    MP_Status_t  (*put_bigint)(MP_Link_pt, MP_ApInt_t);
    MP_Status_t  (*get_bigint)(MP_Link_pt link, MP_ApInt_t *bigint);
    char*        (*bigint_to_str)(MP_ApInt_t bigint, char *buffer);
    long         (*bigint_ascii_size)(MP_ApInt_t bigint);
} MP_BigIntOps_t;
typedef MP_BigIntOps_t * MP_BigIntOps_pt;

EXTERN MP_BigIntOps_t imp_default_bigint_ops;
EXTERN MP_BigNumFormat_t imp_default_bigint_format;

typedef struct   MP_BigRealOps_t
{
    MP_Status_t  (*put_bigreal)(MP_Link_pt link, MP_ApReal_t bigreal);
    MP_Status_t  (*get_bigreal)(MP_Link_pt link, MP_ApReal_t *bigreal);
    char*        (*bigreal_to_str)(MP_ApReal_t bigreal, char *buffer);
    long         (*bigreal_ascii_size)(MP_ApReal_t bigreal);
} MP_BigRealOps_t;
typedef MP_BigRealOps_t * MP_BigRealOps_pt;

EXTERN MP_BigRealOps_t imp_default_bigreal_ops;
EXTERN MP_BigNumFormat_t imp_default_bigreal_format;

typedef struct  MP_BigNum_t
{
    MP_BigNumFormat_t native_bigint_format;
    MP_BigIntOps_pt   bigint_ops;
    MP_BigNumFormat_t native_bigreal_format; 
    MP_BigRealOps_pt  bigreal_ops; 
} MP_BigNum_t;

EXTERN MP_Status_t IMP_PutDummyBigInt(MP_Link_pt link, MP_ApInt_t dummy);
EXTERN MP_Status_t IMP_GetDummyBigInt(MP_Link_pt link, MP_ApInt_t *dummy);
EXTERN char* IMP_DummyBigIntToStr(MP_ApInt_t dummy, char *buf);
EXTERN long IMP_DummyBigIntAsciiSize(MP_ApInt_t dummy);
EXTERN MP_Status_t IMP_PutDummyBigReal(MP_Link_pt link, MP_ApReal_t dummy);
EXTERN MP_Status_t IMP_GetDummyBigReal(MP_Link_pt link, MP_ApReal_t *dummy);
EXTERN char* IMP_DummyBigRealToStr(MP_ApReal_t dummy, char *buf);
EXTERN long IMP_DummyBigRealAsciiSize(MP_ApReal_t dummy);


#endif /* _MP_BigNum_h */

/* Stuff included from h/MP_Env.h */
/***************************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Env.h 
 * 
 *        Declarations to maintain the MP environment.  The environment
 *        includes the name of the host machine, the log file name and
 *      file descriptor and the buffer pool containing all the buffers
 *      to be shared among the links.  The initialization should be
 *      done once before any MP-related activities occur.
 *                                                                
 *  Change Log:
 *       September 10, 1995  SG - Updated implementation.  Much cleaning
 *                                to make it presentable.
 *       November 25, 1995   SG - Added field in env struct to support
 *                                negotiation of endian order.
 *       April 32, 1996      SG - Added support to handle different
 *                                Big Integer packages.  See note in
 *                                MP_SacBigInt.c for some details.
 ***************************************************************************/
#ifndef _MP_Env_h
#define _MP_Env_h 



#include "MP.h"

#define MP_HOST_NAME_LEN    64
#define MP_INIT_LOGFILE_EXT 100

/* These are the transport types currently supported   */
/* These are the values stored in transp_dev.         */
/* If you add to this list, you must also add a string */
/* equivalent to transp_dev2string[] in MP_Link.c.    */

#define MP_TcpTransportDev   0
#define MP_FileTransportDev  1
#define MP_PvmTransportDev   2
#define MP_TbTransportDev    3


/*
 * Options that can be set for an environment 
 */
#define MP_BUFFER_SIZE_OPT       1
#define MP_MAX_FREE_BUFFERS_OPT  2
#define MP_INIT_FREE_BUFFERS_OPT 3

typedef struct  {
    long         (*write_buf)();          /* write buffer to transport   */
    long         (*read_buf)();           /* read transport to buffer    */
    MP_Status_t  (*flush_buf)();          /* flush buffer to transport   */
    MP_Boolean_t (*get_status)();         /* check status of the link    */
    MP_Status_t  (*open_transp)();        /* open the transport device   */
    MP_Status_t  (*close_transp)();       /* close the transport device  */
} MP_TranspOps_t;


typedef struct {
    int            transp_dev;
    MP_TranspOps_t *transp_ops;
    char           *private1;    /* for opaque transport-specific structure */
} MP_Transport_t;

typedef struct transp_list_elem *MP_TranspList_pt;

struct transp_list_elem {
    int               transp_dev;
    MP_TranspOps_t    transp_ops;
    MP_TranspList_pt *next;
};


typedef struct mp_environment {
    MP_TranspList_pt transp_dev_list; /* list of supported transport devices */
    buffer_pool_pt   buff_pool;       /* buffer pool shared by  links in this
                                       *  env  
                                       */
    int   num_o_buff;
    int   buffer_size;                /* size of each buffer in the pool     */
    int   max_free_buffers;           /* max number of free buffers in the 
                                       * pool
                                       */

    int   init_free_buffers;          /* initial number of free buffers in the
                                       * pool 
                                       */

    int   num_links;                  /* to handout unique link ids for this
                                       * env
                                       */

    FILE  *logfd;                     /* log file for all logging events     */
    char  *logfilename;
    char  thishost[MP_HOST_NAME_LEN];
    MP_Boolean_t initialized;         /* can't set options after env has been 
                                       * init
                                       */

    MP_WordOrder_t native_word_order; /* what this machine uses internally   */
    MP_FpFormat_t  native_fp_format;  /* floating pnt format used internally */
    MP_BigNum_t    bignum;            /* info on bignum package to be used   */
} MP_Env_t;

typedef MP_Env_t *MP_Env_pt;
typedef MP_Env_pt MP_ENV;

EXTERN MP_ENV MP_AllocateEnv _ANSI_ARGS_((void));
EXTERN MP_ENV MP_InitializeEnv _ANSI_ARGS_((MP_Env_pt env));
EXTERN void MP_ReleaseEnv _ANSI_ARGS_((MP_Env_pt env));
EXTERN int MP_SetEnvOption _ANSI_ARGS_((MP_Env_pt env, int option, int value));
EXTERN int    MP_GetEnvOption _ANSI_ARGS_((MP_Env_pt env, int option));
EXTERN MP_TranspList_pt IMP_GetTranspByName _ANSI_ARGS_((MP_Env_pt env, 
                                                         int transp_dev));
EXTERN MP_Status_t MP_AddEnvTranspDevice _ANSI_ARGS_((MP_Env_pt env, 
                                        int transp_dev, MP_TranspOps_t *ops));
EXTERN MP_Status_t MP_SetEnvBigIntFormat _ANSI_ARGS_((MP_Env_t *env, 
                             MP_BigIntOps_t *ops, MP_BigNumFormat_t format));
EXTERN MP_Status_t MP_SetEnvBigRealFormat _ANSI_ARGS_((MP_Env_t *env, 
                             MP_BigRealOps_t *ops, MP_BigNumFormat_t format));
EXTERN MP_Status_t open_logfile _ANSI_ARGS_((MP_ENV env));
#endif /* _MP_Env_h */

/* Stuff included from h/MP_Link.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Link.h
 *
 *   struct link is adapted from the Sun XDR distribution.
 *   Copyright (C) 1984, Sun Microsystems, Inc. 
 *
 ***************************************************************/

#ifndef _MP_Link_h
#define _MP_Link_h

/* Sending modes */
#define MP_SEND_FRAG_MSG      1
#define MP_SEND_WHOLE_MSG     2
#define MP_DEFAULT_SEND_MODE  MP_SEND_FRAG_MSG 

/* Receiving modes */
#define MP_RECV_FRAG_MSG_RECVER   1
#define MP_RECV_FRAG_MSG_SENDER   2
#define MP_RECV_WHOLE_MSG         3
#define MP_DEFAULT_RECV_MODE      MP_RECV_FRAG_MSG_RECVER 

/* Options that can be set for a link */
#define MP_LINK_SEND_MODE_OPT 1
#define MP_LINK_RECV_MODE_OPT 2
#define MP_LINK_LOG_MASK_OPT  3

#define  eor_mask ((unsigned long)(1 << 31))


struct MP_Link_t
{
    int               link_id;
    int               errno;
    MP_Env_pt         env;
    MP_Transport_t    transp;
    MP_BigNum_t       bignum;       /* bignum type & fncs struct  */
    int               logmask;
    buffer_handle_pt  i_buff;       /* ptr to list of input buff  */
    buffer_handle_pt  o_buff;       /* ptr to list of output buff */
    buffer_handle_pt  curr_o_buff;  /* current output buffer      */
    buffer_handle_pt  curr_i_buff;  /* current input buffer       */
    int               num_o_buff;   /* number of o_buff to keep with stream */

    int snd_mode; /* Two possible values:
                   * MP_SEND_FRAG_MSG - send a fragment at a time
                   * MP_SEND_WHOLE_MSG - send a tree at a time
                   */

    int rcv_mode; /* Three possible value:
                   * MP_RECV_FRAG_MSG_RECVER - read one sender's
                   * fragment at a time,
                   * MP_RECV_FRAG_MSG_SENDER - read one reciever's
                   * fragment at a time,
                   * MP_RECV_WHOLE_MSG - read a tree at a time.
                   */
    /*
     * out-going bits
     */
    char   *out_base;         /* output buffer (points to frag header) */
    char   *out_finger;       /* next output position */
    char   *out_boundry;      /* data can go up to this address */
    unsigned long  *o_frag_header;    /* beginning of current fragment */

    /*
     * in-coming bits
     */
    char   *in_base;          /* input buffer (points to frag header) */
    char   *in_finger;        /* location of next byte to be had */
    char   *in_boundry;       /* can read up to this location */
    unsigned long  *i_frag_header;    /* points to fragment header */

    long         fbtbc;        /* fragment bytes to be consumed */
    long         sfblr;        /* sender's fragment bytes left to be read */
    MP_Boolean_t last_frag;    /* tell if curr. input buff. is last of tree */
    MP_Boolean_t s_last_frag;  /* tell if current input buffer is part of
                                  sender's last frag. of tree */

    /* the next three are inherited from the link's  environment, but
     * are resettable and, in some cases, negotiated with the partner
     */
    MP_WordOrder_t    link_word_order;
    MP_FpFormat_t     link_fp_format;
    MP_BigNumFormat_t link_bigint_format;   /* format of SENDER! */
    MP_BigNumFormat_t link_bigreal_format;
};

#define TEST_LINK_NULL(link) \
   if (link == NULL) return (MP_NullLink)

EXTERN MP_Link_pt MP_OpenLink _ANSI_ARGS_((MP_Env_pt env, int argc,
                                             char **argv));

EXTERN void MP_CloseLink _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_PeekHeader _ANSI_ARGS_((MP_Link_pt link,
                                              MP_NodeType_t *ntype,
                                              MP_DictTag_t *dtag,
                                              MP_Common_t *cval));

EXTERN MP_Status_t MP_InitMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_SkipMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_EndMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Status_t MP_EndMsgReset _ANSI_ARGS_((MP_Link_pt link));

EXTERN MP_Boolean_t MP_TestEofMsg _ANSI_ARGS_((MP_Link_pt link));

EXTERN int MP_SetLinkOption _ANSI_ARGS_((MP_Link_pt link, int option,
                                         int value));

EXTERN int MP_GetLinkOption _ANSI_ARGS_((MP_Link_pt link, int option));

EXTERN MP_Boolean_t MP_GetLinkStatus _ANSI_ARGS_((MP_Link_pt link,
                                            MP_LinkStatus_t status_to_check));

EXTERN MP_Status_t IMP_GetLong _ANSI_ARGS_((MP_Link_pt link, long *lp));

EXTERN MP_Status_t IMP_PutLong _ANSI_ARGS_((MP_Link_pt link, long *lp));

EXTERN MP_Status_t IMP_GetBytes _ANSI_ARGS_((MP_Link_pt link, char *addr,
                                             unsigned long len));

EXTERN MP_Status_t IMP_PutBytes _ANSI_ARGS_((MP_Link_pt link, char *addr,
                                             unsigned long len));

EXTERN MP_Status_t IMP_Restore _ANSI_ARGS_((MP_Link_pt link, unsigned long n));
#endif  /* _MP_Link_h */

/* Stuff included from h/MP_Buffer.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Buffer.h
 *
 *       Contains all declarations related to buffer and buffer pool.
 *
 ****************************************************************/
#ifndef _MP_Buffer_h
#define _MP_Buffer_h

#ifndef _MP_BuffTypes_h
#include "MP_BuffTypes.h"
#endif

EXTERN MP_Status_t m_create_pool _ANSI_ARGS_((MP_Env_pt env, int buffer_size,
                                       int max_free_buff, int init_free_buff));
EXTERN void m_free_pool     _ANSI_ARGS_((buffer_pool_pt buff_pool));
EXTERN void m_free_in_bufs  _ANSI_ARGS_((MP_Link_pt link));
EXTERN int MP_ResetLink     _ANSI_ARGS_((MP_Link_pt link));
EXTERN void destroy_buffer  _ANSI_ARGS_((buffer_handle_pt buff, long buffsize));
EXTERN void free_buffer     _ANSI_ARGS_((buffer_pool_pt buff_pool,
                                         buffer_handle_pt buff));
EXTERN buffer_handle_pt create_buffer _ANSI_ARGS_((buffer_pool_pt buff_pool));
EXTERN buffer_handle_pt get_buffer    _ANSI_ARGS_((buffer_pool_pt buff_pool));
EXTERN buffer_handle_pt get_o_buff    _ANSI_ARGS_((MP_Link_pt link));
EXTERN buffer_handle_pt get_i_buff    _ANSI_ARGS_((MP_Link_pt link));
EXTERN MP_Status_t reset_i_buff       _ANSI_ARGS_((MP_Link_pt link));

#endif /* _MP_Buffer_h */

/* Stuff included from h/MP_Util.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Util.h
 *
 ***************************************************************/

#ifndef _MP_Util_h
#define _MP_Util_h

/* defines for logging events */
#define MP_ERROR_EVENT   "ErrorE "
#define MP_WRITE_EVENT   "WriteE "
#define MP_READ_EVENT    "ReadE  "
#define MP_INIT_EVENT    "InitE  "
#define MP_URGENT_EVENT  "UrgentE"
#define MP_CONTROL_EVENT "CntrlE "

/* 
 * Logging is done on a per link basis.  The user is allowed to 
 * specify which events to log.  The default is to log all events.
 * Error events are always logged and cannot be disabled.
 */

#define MP_LOG_READ_EVENTS         1
#define MP_LOG_WRITE_EVENTS        2
#define MP_LOG_INIT_EVENTS         4
#define MP_LOG_CONTROL_EVENTS      8
#define MP_LOG_URGENT_EVENTS      16
#define MP_LOG_ALL_EVENTS         31
#define MP_LOG_NO_EVENTS           0
#define MP_LOG_NO_READ_EVENTS     ~MP_LOG_READ_EVENTS
#define MP_LOG_NO_WRITE_EVENTS    ~MP_LOG_WRITE_EVENTS
#define MP_LOG_NO_INIT_EVENTS     ~MP_LOG_INIT_EVENTS
#define MP_LOG_NO_CONTROL_EVENTS  ~MP_LOG_CONTROL_EVENTS
#define MP_LOG_NO_URGENT_EVENTS   ~MP_LOG_URGENT_EVENTS

#define log_msg_len 128

EXTERN void MP_LogEvent _ANSI_ARGS_((MP_Link_pt link, char *event, char *msg));

EXTERN MP_Status_t MP_SetError _ANSI_ARGS_((MP_Link_pt link, 
                                            MP_Status_t the_err));

#define MP_ClearError(link) (link)->errno = MP_Success

EXTERN void MP_PrintError _ANSI_ARGS_((MP_Link_pt link));

EXTERN char* MP_ErrorStr _ANSI_ARGS_((MP_Link_pt link));

EXTERN char *IMP_GetCmdlineArg _ANSI_ARGS_((int argc, char **argv, char *cmd));

EXTERN char *IMP_TypeToString  _ANSI_ARGS_((MP_NodeType_t t));

/* the following are used internally by MP for logging */

EXTERN void log_dicttype _ANSI_ARGS_((MP_Link_pt link, char *event,
                                      char *type_str, MP_NodeType_t type,
                                      MP_NumAnnot_t na, MP_DictTag_t dtag,
                                      void *data, MP_NumChild_t nc));

EXTERN void log_fixnum _ANSI_ARGS_((MP_Link_pt link, char *event,
                                    char *type_str, int type, MP_NumAnnot_t na,
                                    void *val));

EXTERN void log_fixreal _ANSI_ARGS_((MP_Link_pt link, char *event,
                                     char *type_str, int type, MP_NumAnnot_t na,
                                     void *val));

EXTERN void annot_flags_to_str _ANSI_ARGS_((MP_AnnotFlags_t flags));

#endif /* _MP_Util_h */

/* Stuff included from h/MP_Put.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Put.h
 *                        
 ***************************************************************/

#ifndef _MP_Put_h
#define _MP_Put_h

#include "MP.h"

EXTERN MP_Status_t  MP_PutAnnotationPacket _ANSI_ARGS_((MP_Link_pt link,
                                                        MP_DictTag_t dtag,
                                                        MP_AnnotType_t atype, 
                                                        MP_AnnotFlags_t flags));

EXTERN MP_Status_t  MP_PutApIntPacket _ANSI_ARGS_((MP_Link_pt link, 
                                                   MP_ApInt_t apint,
                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutApRealPacket _ANSI_ARGS_((MP_Link_pt link, 
                                                    MP_ApReal_t apreal, 
                                                    MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutSint32Packet _ANSI_ARGS_((MP_Link_pt link,
                                                    MP_Sint32_t  n, 
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutUint32Packet _ANSI_ARGS_((MP_Link_pt link, 
                                                    MP_Uint32_t  n, 
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutSint8Packet _ANSI_ARGS_((MP_Link_pt link,
                                                   MP_Sint8_t n, 
                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutUint8Packet _ANSI_ARGS_((MP_Link_pt link, 
                                                   MP_Uint8_t n,
                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutBooleanPacket _ANSI_ARGS_((MP_Link_pt link, 
                                                     MP_Boolean_t n,
                                                     MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutReal32Packet _ANSI_ARGS_((MP_Link_pt link, 
                                                    MP_Real32_t n, 
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutReal64Packet _ANSI_ARGS_((MP_Link_pt link, 
                                                    MP_Real64_t n, 
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                                                      MP_DictTag_t dtag, 
                                                      char *op, 
                                                      MP_NumAnnot_t num_annots, 
                                                      MP_NumChild_t num_child));
EXTERN MP_Status_t  MP_PutMetaOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
						      MP_DictTag_t dtag,
						      char *op,
						      MP_NumAnnot_t num_annots,
						      MP_NumChild_t num_child));

EXTERN MP_Status_t  MP_PutCommonOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                                                            MP_DictTag_t dtag,
                                                            MP_Common_t op,
                                                            MP_NumAnnot_t num_annots, 
                                                            MP_NumChild_t num_child));

EXTERN MP_Status_t  MP_PutCommonMetaOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
							    MP_DictTag_t dtag,
							    MP_Common_t op,
							    MP_NumAnnot_t num_annots,
							    MP_NumChild_t num_child));
EXTERN MP_Status_t  MP_PutCommonMetaTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                                        MP_DictTag_t dtag,
                                                        MP_Common_t cm,
                                                        MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutRawPacket _ANSI_ARGS_((MP_Link_pt link,
                                                 char *data, 
                                                 MP_Uint32_t len,
                                                 MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutConstantPacket _ANSI_ARGS_((MP_Link_pt link, 
                                                      MP_DictTag_t dtag,
                                                      char *cnst,
                                                      MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutCommonConstantPacket _ANSI_ARGS_((MP_Link_pt link,
                                                            MP_DictTag_t dtag, 
                                                            MP_Common_t cnst,
                                                            MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                                        MP_DictTag_t dtag,
                                                        char *id,
                                                        MP_NumAnnot_t num_annots));

EXTERN MP_Status_t  MP_PutCommonLatinIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                                                   MP_DictTag_t dtag,
                                                                   MP_Common_t id,
                                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutCommonGreekIdentifierPacket _ANSI_ARGS_((MP_Link_pt link,
                                                                   MP_DictTag_t dtag,
                                                                   MP_Common_t id,
                                                                   MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutStringPacket _ANSI_ARGS_((MP_Link_pt link,
                                                    char *s,
                                                    MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  MP_PutMetaTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                                  MP_DictTag_t dtag, char *s,
                                                  MP_NumAnnot_t  num_annots));

EXTERN MP_Status_t  IMP_PutString _ANSI_ARGS_((MP_Link_pt link,
                                               char *s));

EXTERN MP_Status_t  IMP_PutOperator _ANSI_ARGS_((MP_Link_pt link,
                                                 char *op));

EXTERN MP_Status_t  IMP_PutMetaOperator _ANSI_ARGS_((MP_Link_pt link,
                                                     char *op));

EXTERN MP_Status_t  IMP_PutApInt _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApInt_t apint)); 

EXTERN MP_Status_t  IMP_PutApReal _ANSI_ARGS_((MP_Link_pt link,
                                               MP_ApReal_t apreal));

EXTERN MP_Status_t  IMP_PutBoolean _ANSI_ARGS_((MP_Link_pt link, 
                                                MP_Boolean_t n));

EXTERN MP_Status_t  IMP_PutSint8 _ANSI_ARGS_((MP_Link_pt link,
                                              MP_Sint8_t  n));

EXTERN MP_Status_t  IMP_PutUint8 _ANSI_ARGS_((MP_Link_pt link,
                                              MP_Uint8_t  n));

EXTERN MP_Status_t  IMP_PutSint32 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Sint32_t n));

EXTERN MP_Status_t  IMP_PutUint32 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Uint32_t n));

EXTERN MP_Status_t  IMP_PutReal32 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Real32_t n));

EXTERN MP_Status_t  IMP_PutReal64 _ANSI_ARGS_((MP_Link_pt link,
                                               MP_Real64_t n));

EXTERN MP_Status_t  IMP_PutRaw _ANSI_ARGS_((MP_Link_pt link,
                                            char *rawdata,
                                            MP_Uint32_t len));

EXTERN MP_Status_t  IMP_PutNodeHeader _ANSI_ARGS_((MP_Link_pt link, 
                                                   MP_NodeType_t ntype,
                                                   MP_DictTag_t dtag,
                                                   MP_Common_t cval, 
                                                   MP_NumAnnot_t num_annots,
                                                   MP_NumChild_t num_child));

EXTERN MP_Status_t  IMP_PutStringBasedTypePacket _ANSI_ARGS_((MP_Link_pt link,
                                                              char *s,
                                                              MP_NumAnnot_t num_annots,
                                                              MP_NodeType_t ntype));

/*
*  defines for putting a string-based type to the data stream.
*  The prototype
*        IMP_PutStringBasedTypePacket  (MP_Link_pt link, char *s, 
*                MP_NumAnnot_t  num_annots, MP_NodeType _t ntype)
*  clarifies the type of the arguments.
*/

#define IMP_PutIdentifier(link, id) \
   (IMP_PutString(link, id))

#define IMP_PutMetaType(link, meta) \
   (IMP_PutString(link, meta))

#endif  /* _MP_put_h */

/* Stuff included from h/MP_Get.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Get.h
 *                        
 ***************************************************************/

#ifndef _MP_Get_h
#define _MP_Get_h

EXTERN MP_Status_t  MP_GetAnnotationPacket _ANSI_ARGS_((MP_Link_pt link,
                                MP_DictTag_t *dtag, MP_AnnotType_t *atype,
                                MP_AnnotFlags_t *flags));

EXTERN MP_Status_t  MP_GetApIntPacket _ANSI_ARGS_((MP_Link_pt link, 
                        MP_ApInt_pt apint, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetApRealPacket _ANSI_ARGS_((MP_Link_pt link, 
                        MP_ApReal_pt apreal, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetSint32Packet _ANSI_ARGS_((MP_Link_pt link,
                                    MP_Sint32_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetUint32Packet _ANSI_ARGS_((MP_Link_pt link, 
                                    MP_Uint32_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetSint8Packet _ANSI_ARGS_((MP_Link_pt link, 
                                    MP_Sint8_t *n,  MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetUint8Packet _ANSI_ARGS_((MP_Link_pt link, 
                                     MP_Uint8_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetBooleanPacket _ANSI_ARGS_((MP_Link_pt link, 
                                   MP_Boolean_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetReal32Packet _ANSI_ARGS_((MP_Link_pt link, 
                                    MP_Real32_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetReal64Packet _ANSI_ARGS_((MP_Link_pt link, 
                                    MP_Real64_t *n, MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t  MP_GetOperatorPacket _ANSI_ARGS_((MP_Link_pt link,
                           MP_DictTag_t *dtag, char **op,
                          MP_NumAnnot_t *num_annots, MP_NumChild_t *num_child));

EXTERN MP_Status_t MP_GetCommonOperatorPacket _ANSI_ARGS_((MP_Link_pt link, 
                         MP_DictTag_t *dtag, MP_Common_t *op, 
                         MP_NumAnnot_t *num_annots,  MP_NumChild_t *num_child));

EXTERN MP_Status_t MP_GetCommonMetaTypePacket _ANSI_ARGS_((MP_Link_pt link, 
                                        MP_DictTag_t *dtag, MP_Common_t *cmt, 
                                                  MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetIdentifierPacket _ANSI_ARGS_((MP_Link_pt link, 
                                              MP_DictTag_t *dtag, char **id, 
                                              MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetCommonLatinIdentifierPacket _ANSI_ARGS_((MP_Link_pt 
                                 link,  MP_DictTag_t *dtag, MP_Common_t *id,
                                 MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetCommonGreekIdentifierPacket _ANSI_ARGS_((MP_Link_pt
                             link, MP_DictTag_t *dtag, MP_Common_t *id, 
                             MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetStringPacket _ANSI_ARGS_((MP_Link_pt link, char **s,
                                                          MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetConstantPacket _ANSI_ARGS_((MP_Link_pt link, 
                                 MP_DictTag_t *dtag, char **cnst, 
                                 MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetCommonConstantPacket _ANSI_ARGS_((MP_Link_pt link,
                            MP_DictTag_t *dtag, MP_Common_t *cnst, 
                            MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetMetaTypePacket _ANSI_ARGS_((MP_Link_pt link, 
                        MP_DictTag_t *dtag, char **m,
                        MP_NumAnnot_t *num_annots));

EXTERN MP_Status_t MP_GetRawPacket _ANSI_ARGS_((MP_Link_pt link, char **rawdata,
                        MP_Uint32_t *len,  MP_NumAnnot_t *num_annots));
EXTERN MP_Status_t IMP_GetUint8 _ANSI_ARGS_((MP_Link_pt link, MP_Uint8_t *n));
EXTERN MP_Status_t IMP_GetSint8 _ANSI_ARGS_((MP_Link_pt link, MP_Sint8_t *n));
EXTERN MP_Status_t IMP_GetBoolean _ANSI_ARGS_((MP_Link_pt link, MP_Boolean_t *n));
EXTERN MP_Status_t IMP_GetApInt _ANSI_ARGS_((MP_Link_pt link, MP_ApInt_pt apint));
EXTERN MP_Status_t IMP_GetApReal _ANSI_ARGS_((MP_Link_pt link, MP_ApReal_pt apreal));
EXTERN MP_Status_t IMP_GetReal32 _ANSI_ARGS_((MP_Link_pt link, MP_Real32_t *n));
EXTERN MP_Status_t IMP_GetReal64 _ANSI_ARGS_((MP_Link_pt link, MP_Real64_t *n));
EXTERN MP_Status_t IMP_GetSint32 _ANSI_ARGS_((MP_Link_pt link, MP_Sint32_t *n));
EXTERN MP_Status_t IMP_GetUint32 _ANSI_ARGS_((MP_Link_pt link, MP_Uint32_t *n));

EXTERN MP_Status_t IMP_GetNodeHeader _ANSI_ARGS_((MP_Link_pt link, 
                          MP_NodeType_t *ntype, MP_DictTag_t *dtag,
                          MP_Common_t *cv, MP_NumAnnot_t *num_annots,
                          MP_NumChild_t *num_child));

EXTERN MP_Status_t IMP_GetStringBasedType _ANSI_ARGS_((MP_Link_pt link, 
                                                        char **s));

EXTERN MP_Status_t IMP_GetRaw _ANSI_ARGS_((MP_Link_pt link, char **rawdata,
                                           MP_Uint32_t *len));

EXTERN MP_Status_t IMP_GetOperator _ANSI_ARGS_((MP_Link_pt link, char **op));

/*
 *  defines for getting a string-based type from the data stream.
 */

#define IMP_GetIdentifier(link, id) \
   (IMP_GetStringBasedType(link, id))

#define IMP_GetString(link, str) \
   (IMP_GetStringBasedType(link, str))

#define IMP_GetMetaType(link, meta) \
   (IMP_GetStringBasedType(link, meta))

#define MP_GetCommonMetaOperatorPacket(link, dtag, op, num_annots, num_child) \
   (MP_GetCommonOperatorPacket(link, dtag, op, num_annots, num_child))

#define MP_GetMetaOperatorPacket(link, dtag, op, num_annots, num_child) \
   (MP_GetOperatorPacket(link, dtag, op, num_annots, num_child))

#define IMP_GetMetaOperator(link, op) \
   (IMP_GetStringBasedType(link, op))

#endif /* _MP_Get_h */

/* Stuff included from h/MP_FileTransp.h */
/***************************************************************************
 *                                                                  
 *   HEADER FILE:  MP_FileTransp.h
 * 
 *        Declarations for the "file" transport device.
 *                                                                
 *  Change Log:
 *       September 10, 1995  SG - Updated implementation.  Much cleaning
 *                                to make it presentable.
 *       August 21, 1996     SG - Changed FILE_MODE from 0x666 to 0666.
 *                                Now it behaves properly
 *
 ***************************************************************************/
#ifndef _MP_FileTransp_h
#define _MP_FileTransp_h

#define MP_NO_SUCH_FILE_MODE    0
#define MP_READ_MODE            1
#define MP_WRITE_MODE           2
#define MP_APPEND_MODE          3

typedef struct {
    FILE    *fptr;
    int      access_mode; /* read, write, or append as defined above */
    char    *fname;
} MP_FILE_t;

MP_Status_t file_flush _ANSI_ARGS_((MP_Link_pt link));
long file_write _ANSI_ARGS_((MP_Link_pt link, char * buf, long len));
long file_read _ANSI_ARGS_((MP_Link_pt link,
                            char *buf,
                            long len));
MP_Boolean_t file_get_status _ANSI_ARGS_((MP_Link_pt      link,
                                          MP_LinkStatus_t status_to_check));
MP_Status_t file_open_connection _ANSI_ARGS_((MP_Link_pt link,
                                              int        argc,
                                              char       **argv));
MP_Status_t file_close_connection _ANSI_ARGS_((MP_Link_pt link));
MP_Status_t file_init_transport _ANSI_ARGS_((MP_Link_pt link));
int get_file_mode _ANSI_ARGS_((int  argc,
                               char **argv));
#endif /* _MP_FileTransp_h */

/* Stuff included from h/MP_TcpTransp.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_TcpTransp.h
 *
 *        Isolate all the includes we need for unix TCP sockets.
 *
 *  Change Log:
 *       September 10, 1995  SG - Updated implementation.  Much cleaning
 *                                to make it presentable.
 *
 ***************************************************************************/
#ifndef __TcpTransp__h
#define __TcpTransp__h

#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>

#ifdef __WIN32__

#   include <winsock.h>

#   define  ERRORCODE(err)      WSA ## err
#   define  LASTERROR           WSAGetLastError()

#   define  CLOSE_SOCKET(s)             closesocket(s)
#   define  READ_SOCKET(s, buf, len)    recv(s, buf, len, 0)
#   define  WRITE_SOCKET(s, buf, len)   send(s, buf, len, 0)

int WinSockInitialize _ANSI_ARGS_((void));

#else /* not __WIN32__ */

#   include <sys/socket.h>
#   include <netinet/in.h>
#   include <netinet/tcp.h>
#   include <sys/ioctl.h>
#   include <arpa/inet.h>
#   include <netdb.h>

typedef int SOCKET;

#   define  INVALID_SOCKET      -1
#   define  SOCKET_ERROR        -1
#   define  IPPORT_RESERVED     1024
#   define  ERRORCODE(err)      err
#   define  LASTERROR           errno

#   define  CLOSE_SOCKET(s)             close(s)
#   define  READ_SOCKET(s, buf, len)    read(s, buf, len)
#   define  WRITE_SOCKET(s, buf, len)   write(s, buf, len)

#   define  HOSTENT             struct hostent
#   define  SOCKADDR            struct sockaddr
#   define  TIMEVAL             struct timeval
#   define  SOCKADDR_IN         struct sockaddr_in

#endif /* not __WIN32__ */


#define MP_NO_SUCH_TCP_MODE     0
#define MP_CONNECT_MODE         1
#define MP_LISTEN_MODE          2
#define MP_LAUNCH_MODE          3
#define MP_FORK_MODE            4

/* Initial port for listening */
#define MP_INIT_PORT            "1025"

/* The maximum number of attempts made to bind a socket to a port address */
#define MP_MAX_BIND_ATTEMPTS    1000

#define MP_ACCEPT_WAIT_SEC      5
#define MP_ACCEPT_WAIT_USEC     0

MP_Status_t tcp_flush _ANSI_ARGS_((MP_Link_pt link));
long tcp_write _ANSI_ARGS_((MP_Link_pt, char *, long));
long tcp_read _ANSI_ARGS_((MP_Link_pt, char *, long));
MP_Boolean_t tcp_get_status _ANSI_ARGS_((MP_Link_pt, MP_LinkStatus_t));
MP_Status_t tcp_init_transport _ANSI_ARGS_((MP_Link_pt));
MP_Status_t tcp_open_connection _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t tcp_close_connection _ANSI_ARGS_((MP_Link_pt));
MP_Status_t open_tcp_connect_mode _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t open_tcp_listen_mode _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t open_tcp_launch_mode _ANSI_ARGS_((MP_Link_pt, int, char**));
MP_Status_t socket_accept_blocking _ANSI_ARGS_((MP_Link_pt, SOCKET*));
MP_Status_t socket_accept_non_blocking _ANSI_ARGS_((MP_Link_pt, SOCKET*));
char* IMP_GetCmdlineArg _ANSI_ARGS_((int, char**, char*));


typedef struct{
    SOCKET  bindsock;
    SOCKET  sock;
    short   peerport;
    char    *peerhost;
    char    *myhost;
    char     isparent;
} MP_TCP_t;

#endif

/* Stuff included from h/MP_Vector.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_Vector.h
 *                                                                
 ***************************************************************/

#ifndef _MP_Vector_h
#define _MP_Vector_h

#include "MP.h"

/* Putting a Vector */
EXTERN MP_Status_t IMP_PutBasicVectorHeader
_ANSI_ARGS_((MP_Link_pt link, MP_Common_t elem_type,
              MP_NumAnnot_t na, MP_NumChild_t num_elems));

EXTERN MP_Status_t MP_PutBasicVector
_ANSI_ARGS_((MP_Link_pt link, void *elems,
             MP_Common_t elem_type, MP_NumChild_t num_elems));

EXTERN MP_Status_t IMP_PutBasicVector
_ANSI_ARGS_((MP_Link_pt link, void *elems,
             MP_Common_t elem_type, MP_NumChild_t num_elems));

EXTERN MP_Status_t  IMP_PutReal64Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Real64_t *dvec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_PutUint32Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint32_t *si32vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_PutUint16Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint16_t *si16vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_PutUint8Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint8_t *si8vec, MP_NumChild_t len));

#define IMP_PutReal32Vector(link, vec, len)         \
IMP_PutUint32Vector(link, (MP_Uint32_t *) vec, len)

#define IMP_PutSint32Vector(link, vec, len)         \
IMP_PutUint32Vector(link, (MP_Uint32_t *) vec, len)

#define IMP_PutSint16Vector(link, vec, len)         \
IMP_PutUint16Vector(link, (MP_Uint16_t *) vec, len)

#define IMP_PutSint8Vector(link, vec, len)         \
IMP_PutUint8Vector(link, (MP_Uint8_t *) vec, len)

/* Getting a vector */
EXTERN MP_Status_t MP_GetBasicVector
_ANSI_ARGS_((MP_Link_pt link, void **elems,
             MP_Common_t elem_type, MP_NumChild_t num_elems));

EXTERN MP_Status_t
IMP_GetReal64Vector _ANSI_ARGS_((MP_Link_pt link, 
                                 MP_Real64_t **dvec, MP_NumChild_t len));

EXTERN MP_Status_t
IMP_GetUint32Vector _ANSI_ARGS_((MP_Link_pt link,
                                 MP_Uint32_t **si32vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_GetUint16Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint16_t **si16vec, MP_NumChild_t len));

EXTERN MP_Status_t  IMP_GetUint8Vector
_ANSI_ARGS_((MP_Link_pt link, MP_Uint8_t **si8vec, MP_NumChild_t len));

#define IMP_GetReal32Vector(link, vec, len)         \
IMP_GetUint32Vector(link, (MP_Uint32_t **) vec, len)

#define IMP_GetSint32Vector(link, vec, len)         \
IMP_GetUint32Vector(link, (MP_Uint32_t **) vec, len)

#define IMP_GetSint16Vector(link, vec, len)         \
IMP_GetUint16Vector(link, (MP_Uint16_t **) vec, len)

#define IMP_GetSint8Vector(link, vec, len)         \
IMP_GetUint8Vector(link, (MP_Uint8_t **) vec, len)

#endif /* _MP_Vector_h */

/* Stuff included from h/MP_GmpBigInt.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_GmpBigInt.h
 *
 *  The bigint routines of gmp
 *
 ****************************************************************/

#ifndef _MP_GmpBigInt_h
#define _MP_GmpBigInt_h

#include "MP.h"

#ifdef MP_HAVE_GMP_APINT

#include "gmp.h"

EXTERN MP_BigIntOps_t imp_gmp_bigint_ops;

EXTERN MP_Status_t IMP_GetGmpInt _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApInt_t* mp_apint));
EXTERN MP_Status_t IMP_PutGmpInt _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApInt_t mp_apint));
EXTERN char *IMP_GmpIntToStr _ANSI_ARGS_((MP_ApInt_t mp_apint,
                                          char *buffer));
EXTERN long IMP_GmpIntAsciiSize _ANSI_ARGS_((MP_ApInt_t mp_apint));

#endif /* MP_HAVE_GMP_APINT */ 

#endif /* _MP_GmpBigInt_h */

/* Stuff included from h/MP_GmpBigReal.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_GmpBigReal.h
 *
 *  The bigreal routines of gmp
 *
 ****************************************************************/

#ifndef _MP_GmpBigReal_h
#define _MP_GmpBigReal_h

#include "MP.h"

#ifdef MP_HAVE_GMP_APREAL
#include "gmp.h"

EXTERN MP_BigRealOps_t imp_gmp_bigreal_ops;

EXTERN MP_Status_t IMP_GetGmpReal _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApReal_t* mp_apreal));
EXTERN MP_Status_t IMP_PutGmpReal _ANSI_ARGS_((MP_Link_pt link,
                                              MP_ApReal_t mp_apreal));
EXTERN char *IMP_GmpRealToStr _ANSI_ARGS_((MP_ApReal_t mp_apreal,
                                          char *buffer));
EXTERN long IMP_GmpRealAsciiSize _ANSI_ARGS_((MP_ApReal_t mp_apreal));

#endif /* MP_HAVE_GMP_APREAL */
#endif /* _MP_GmpBigReal_h */

/* Stuff included from h/MP_PariBigInt.h */
/****************************************************************
 *                                                                  
 * HEADER FILE:  MP_PariBigInt.h
 * Authors:  O. Bachmann, T. Metzner, H. Schoenemann, A. Sorgatz
 * Date:    February 97
 *                                                                
 ***************************************************************/

#ifndef _MP_PariBigInt_h
#define _MP_PariBigInt_h

#include "MP.h"
#ifdef MP_HAVE_PARI

#include "genpari.h"
  
EXTERN MP_BigIntOps_t imp_pari_bigint_ops;

EXTERN MP_Status_t IMP_GetPariBigInt _ANSI_ARGS_((MP_Link_pt link,
                                                 MP_ApInt_t *pari_number));
EXTERN MP_Status_t IMP_PutPariBigInt _ANSI_ARGS_((MP_Link_pt link, 
                                                 MP_ApInt_t pari_number));
EXTERN char* IMP_PariBigIntToStr _ANSI_ARGS_((MP_ApInt_t pari_number,
                                              char *buffer));
EXTERN long IMP_PariBigIntAsciiSize _ANSI_ARGS_((MP_ApInt_t mp_number));

#ifdef HAVE_GMP_PARI_CONVERSIONS
#include "gmp.h"
EXTERN mpz_ptr _pari_to_gmp(GEN pnum, mpz_ptr *gnum_ptr);
EXTERN GEN     _gmp_to_pari(mpz_ptr gnum);
#endif

#endif /* MP_HAVE_PARI */

#endif  /* _MP_PariBigInt_h */

/* Stuff included from h/MP_PvmTransp.h */
/***************************************************************************
 *                                                                  
 *   HEADER FILE:  MP_PvmTransp.h
 * 
 *
 *  Change Log:
 *      1/25/96 sgray - formal creation of PVM transport device.
 *
 ***************************************************************************/
#ifndef __PvmTransp__h
#define __PvmTransp__h

#include "MP_Config.h"

#ifdef MP_HAVE_PVM
#include "pvm3.h"

#define MP_NO_SUCH_PVM_MODE    0

typedef struct{
  int     nhosts;
  int     ntids;  /* the number of tids in next field */
  int     *tids;
  int     tag;
  int     send_mode;
  char    *myhost;
} MP_PVM_t;

/*#include "/tmp_mnt/home/Owl/sgray/MP/MP/MP_PvmTransp.c" */ 

#endif /*  MP_HAVE_PVM */

#endif

/* Stuff included from h/MP_SacBigInt.h */
/****************************************************************
 *                                                                  
 *   HEADER FILE:  MP_SacBigInt.h
 *                                                                
 ***************************************************************/

#ifndef _MP_SacBigInt_h
#define _MP_SacBigInt_h

#include "MP.h"

#ifdef MP_HAVE_SAC
#include "saclib.h"
#include "gmp.h"

EXTERN MP_BigIntOps_t sac_ops;

EXTERN MP_Status_t IMP_PutSacBigInt _ANSI_ARGS_((MP_Link_pt link, 
                                                 MP_ApInt_t sac_int));

EXTERN MP_Status_t IMP_GetSacBigInt _ANSI_ARGS_((MP_Link_pt link,
                                                 MP_ApInt_t *sac_int));

EXTERN char * IMP_SacBigIntToStr _ANSI_ARGS_((MP_ApInt_t big_int, 
                                             char *buffer));

EXTERN long IMP_SacBigIntAsciiSize _ANSI_ARGS_((MP_ApInt_t mp_apint));

#endif /* MP_HAVE_SAC */

#endif  /* _MP_SacBigInt_h */

/* Stuff included from h/MP_TbTransp.h */
/***************************************************************************
 *                                                                  
 *   HEADER FILE:  MP_TbTransp.h
 * 
 *   PURPOSE: ToolBus stuff - not much to do.
 *
 *   Change Log:
 *      2/6/96 sgray - creation of TB transport device.
 *      6/9/96 sgray - updated tb struct to support buffering of terms
 *                     consistent with what the buffering layer expects.
 *
 ***************************************************************************/
#ifndef __TbTransp__h
#define __TbTransp__h

#include "MP_Config.h"

#ifdef MP_HAVE_TB
#include "TB.h"


typedef struct{
  term    *write_term,
          *read_term;
  char    *term_buf;     /* where next binary TB term will be unloaded    */
  char    *next_byte;    /* next byte to be consumed in term_buf          */
  unsigned long buf_len; /* number of bytes in term_buf                   */
  unsigned long tbtbc;   /* Term Bytes To Be Consumed in term_buf         */
  char    *myhost;       /* host on which we are running - must have this */
} MP_TB_t;

EXTERN MP_Status_t MP_TbPutMsg _ANSI_ARGS_((MP_Link_pt link, char **tb_data, 
                                            unsigned long *len)); 
EXTERN MP_Status_t MP_TbGetMsg _ANSI_ARGS_((MP_Link_pt link, char *tb_data, 
                                            unsigned long len)); 
EXTERN MP_Status_t MP_TbSetTerm _ANSI_ARGS_((MP_Link_pt link, term *t));

/* 
 * if your device may not be available everywhere you want to use MP,
 * don't try to compile the source into the library as the link will
 * fail.  The alternative is to include the transport device interface
 * source here and have the user include this header file in their
 * application code.  Then the corresponding .c file will be compiled
 * with the application
 */
/* #include "../MP/MP_TbTransp.c" */

#endif /* MP_HAVE_TB */

#endif

/* MP Dictionary includes */

/* Stuff included from h/MP_BasicDict.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_BasicDict.h                                         
 *                                                                
 *      The MP basic dictionary (definitions and declarations)
 *      
 *        
 *   Change Log:
 *      1/97 obachman  -- this is a first try
 *
 ***************************************************************************/
#ifndef _MP_BasicDict_h
#define _MP_BasicDict_h



/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
/* for the time being, no attention was paid to "good" numeric value
   assignments */
#define MP_CopBasicInc              1
#define MP_CopBasicDec              2
#define MP_CopBasicSize             3
#define MP_CopBasicList             4
#define MP_CopBasicAdd              5
#define MP_CopBasicMinus            6
#define MP_CopBasicMult             7
#define MP_CopBasicDiv              8
#define MP_CopBasicMod              9
#define MP_CopBasicPow              10
#define MP_CopBasicGreaterEqual     11
#define MP_CopBasicGreater          12
#define MP_CopBasicLessEqual        13
#define MP_CopBasicLess             14
#define MP_CopBasicAnd              15
#define MP_CopBasicOr               16
#define MP_CopBasicEqual            17    
#define MP_CopBasicNotEqual         18
#define MP_CopBasicRange            19
#define MP_CopBasicIndex            20
#define MP_CopBasicDiff             21
#define MP_CopBasicInterSect        22        
#define MP_CopBasicSubst            23
#define MP_CopBasicNot              24
#define MP_CopBasicAssign           25


#endif /* _MP_BasicDict_h */

/* Stuff included from h/MP_MpDict.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_MpDict.h
 *
 *      The MP dictionary for (definitions and declarations)
 *
 *
 *   Change Log:
 *        3/26/97  sgray - Life begins for this file.
 *
 ***************************************************************************/
#ifndef _MP_MpDict_h
#define _MP_MpDict_h

/* Let's give the MP Dictionary the number 2 */

#define MP_MpDict 2

/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
#define MP_CopMpEndSession               1

#define MP_CopMpByteOrderRequest        10
#define MP_CopMpFpFormatRequest         11
#define MP_CopMpBigIntFormatRequest     12

/***************************************************************************
 *
 * Annotations
 *
 ***************************************************************************/
#define MP_AnnotMpLabel                 1
#define MP_AnnotMpRef                   2
#define MP_AnnotMpStore                 3
#define MP_AnnotMpStored                4
#define MP_AnnotMpRetrieve              5
#define MP_AnnotMpSource                6
#define MP_AnnotMpComment               7
#define MP_AnnotMpTiming                8
#define MP_AnnotMpType                  9
#define MP_AnnotMpUnits                10

#endif /* _MP_MpDict_h  */

/* Stuff included from h/MP_NumberDict.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_NumberDict.h                                         
 *                                                                
 *      The Numbertype dictionary (definitions and declarations)
 *      
 *        
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_NumberDict_h
#define _MP_NumberDict_h

/* Lets give the Number Dictionary the number 12 */

#define MP_NumberDict 12

/***************************************************************************
 *
 * Meta type definitions
 *
 ***************************************************************************/
#define MP_CmtNumberRational           1

/***************************************************************************
 *
 * Annot definitions
 *
 ***************************************************************************/
#define MP_AnnotNumberIsNormalized          1
#define MP_AnnotNumberIsPrime               2
#define MP_AnnotNumberModulos               3


/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
#define MP_CopNumberPrime               1
#define MP_CopNumberExtGcd              2
#define MP_CopNumberGcd                 3
#define MP_CopNumberRandom              4


#endif /* _MP_NumberDict_h */

/* Stuff included from h/MP_ProtoDict.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_ProtoDict.h                                         
 *                                                                
 *      The prototype dictionary (definitions and declarations)
 *      
 *        
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_ProtoDict_h
#define _MP_ProtoDict_h

#define MP_ProtoDict 6

/***************************************************************************
 *
 * Meta type definitions
 *
 ***************************************************************************/
#define MP_CmtProtoIMP_Sint8                        65
#define MP_CmtProtoIMP_Uint8                        67
#define MP_CmtProtoIMP_Boolean                      68
#define MP_CmtProtoIMP_LatinIdentifier              72
#define MP_CmtProtoIMP_GreekIdentifier              76
#define MP_CmtProtoIMP_CommonConstant               80

#define MP_CmtProtoIMP_String                       36
#define MP_CmtProtoIMP_Constant                     40
#define MP_CmtProtoIMP_Identifier                   44

#define MP_CmtProtoIMP_Sint32                       17
#define MP_CmtProtoIMP_Uint32                       19
#define MP_CmtProtoIMP_Real32                       18

#define MP_CmtProtoIMP_ApInt                        9
#define MP_CmtProtoIMP_ApReal                       10

#define MP_CmtProtoIMP_Real64                       6

#define MP_CmtProtoRecStruct   128
#define MP_CmtProtoRecUnion    129

/* Basic types, i.e. those which appear as a leaf of a data tree
   have numerical values between 0 and 127 ,i.e. bit 8 is zero */
#define IMP_IsBasicMetaType(type)    (!((type) & 128))

/* native 8-bit types have numeric values between 64 and 127
   i.e. bit 8 is zero and bit 7 is one */
#define IMP_Is8BitNumericMetaType(type)    (!((type) & 128) && ((type) & 64))

/* native String-based types have numeric values between 32 an 63,
   i.e. bit 8,7 are zero, bit 6 is one */
#define IMP_IsStringBasedMetaType(type)  (!((type) & 192) && ((type) & 32))

/* native 32 bit types have numeric values beween 16 and 31, i.e. bits
   8,7,6, are zero, bit 5 is one. Furthermore, integers have bit 1 set
   to 1 and Reals have bit 1 set to 0 */
#define IMP_Is32BitNumericMetaType(type)    (!((type) & 224) && ((type) & 16))
#define IMP_Is16BitNumericMetaType(type) 0
/* Ap Numbers have bit 5-8 set to zero, bit 4 set to one */
#define IMP_IsApNumberMetaType(type) (!((type) & 240) && ((type) & 8))

/* Native 64-bit numbers have bit 4-8 set to zero, bit 3 set to one */
#define IMP_Is64BitNumericMetaType(type)    (type == MP_CmtProtoIMP_Real64)

/* Predicates for real and integers
   Integers are odd numbers, reals are even numbers, not divisble by 4*/
#define IMP_IsRealMetaType(type)   (IMP_IsBasicMetaType(type) &&    \
                                 !((type) & 1) &&           \
                                 ((type) >> 1) & 1)
#define IMP_IsIntegerMetaType(type)(IMP_IsBasicMetaType(type) && ((type) & 1))

/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/
#define MP_CopProtoStruct                1
#define MP_CopProtoUnion                 2
#define MP_CopProtoArray                 3
#define MP_CopProtoPointer               4
#define MP_CopProtoRecStruct             5
#define MP_CopProtoRecUnion              6

/***************************************************************************
 *
 * Definition of Annots
 *
 ***************************************************************************/
#define MP_AnnotProtoPrototype           1

#endif /* _MP_ProtoDict_h */

/* Stuff included from h/MP_PolyDict.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_PolyDict.h                                         
 *                                                                
 *      The Polytype dictionary (definitions and declarations)
 *      
 *        
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_PolyDict_h
#define _MP_PolyDict_h

/* Lets give the Poly Dictionary the number 10 */

#define MP_PolyDict 10

/***************************************************************************
 *
 * Annotations
 *
 ***************************************************************************/
#define MP_AnnotPolyModuleVector        1
#define MP_AnnotPolyVarNames            2
#define MP_AnnotPolyVarNumber           3
#define MP_AnnotPolyOrdering            4
#define MP_AnnotPolyDefRel              5
#define MP_AnnotPolyIrreducible         6
#define MP_AnnotPolyWeights             8

/***************************************************************************
 *
 * (Common) Constants
 *
 ***************************************************************************/
#define MP_CcPolyOrdering_No    0
#define MP_CcPolyOrdering_a     1
#define MP_CcPolyOrdering_c     2
#define MP_CcPolyOrdering_C     3
#define MP_CcPolyOrdering_M     4
#define MP_CcPolyOrdering_lp    5
#define MP_CcPolyOrdering_Dp    6
#define MP_CcPolyOrdering_wp    7
#define MP_CcPolyOrdering_Wp    8
#define MP_CcPolyOrdering_ls    9
#define MP_CcPolyOrdering_ds    10
#define MP_CcPolyOrdering_Ds    11
#define MP_CcPolyOrdering_ws    12
#define MP_CcPolyOrdering_Ws    13
#define MP_CcPolyOrdering_Unspec 14



/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/

#define MP_CopPolyBetti                 1
#define MP_CopPolyChar                  2
#define MP_CopPolyCharSeries            3
#define MP_CopPolyDeg                   4
#define MP_CopPolyDim                   5
#define MP_CopPolyFactorize             6
#define MP_CopPolyHilb                  7
#define MP_CopPolyHomog                 8
#define MP_CopPolyInDepSet              9
#define MP_CopPolyIdeal                 10
#define MP_CopPolyKbase                 11
#define MP_CopPolyLead                  12
#define MP_CopPolyDictLeadCoef          13
#define MP_CopPolyDictLeadExp           14
#define MP_CopPolyMaxIdeal              15
#define MP_CopPolyMinBase               16
#define MP_CopPolyMinRes                17
#define MP_CopPolyModule                18
#define MP_CopPolyMultiplicity          19
#define MP_CopPolyOrder                 20
#define MP_CopPolyPrune                 21
#define MP_CopPolyQHWeight              22
#define MP_CopPolyRegularity            23
#define MP_CopPolyStd                   24
#define MP_CopPolySyz                   25
#define MP_CopPolyVdim                  26
#define MP_CopPolyCoeffs                27
#define MP_CopPolyContract              28
#define MP_CopPolyEliminate             29
#define MP_CopPolyJet                   30
#define MP_CopPolyLift                  31
#define MP_CopPolyLiftstd               32
#define MP_CopPolyModulo                33
#define MP_CopPolyMres                  34
#define MP_CopPolyQuotient              35
#define MP_CopPolyReduce                36
#define MP_CopPolyPreimage              37
#define MP_CopPolyRes                   38
#define MP_CopPolyRing                  39
#define MP_CopPolyDenseDistPoly         40
#define MP_CopPolyDenseDistPolyFrac     41
#define MP_CopPolyClearDenom            42
#define MP_CopPolyFacStd                43
#define MP_CopPolyResultant             44


#endif /* _MP_PolyDict_h */

/* Stuff included from h/MP_MatrixDict.h */
/***************************************************************************
 *
 *   HEADER FILE:  MP_MatrixDict.h                                         
 *                                                                
 *      The Matrixtype dictionary (definitions and declarations)
 *      
 *        
 *   Change Log:
 *
 *
 ***************************************************************************/
#ifndef _MP_MatrixDict_h
#define _MP_MatrixDict_h

/* Lets give the Matrix Dictionary the number 11 */

#define MP_MatrixDict 11

/***************************************************************************
 *
 * Annotations
 *
 ***************************************************************************/
#define MP_AnnotMatrixDimension     1


/***************************************************************************
 *
 * Common Operator Definitions
 *
 ***************************************************************************/

#define MP_CopMatrixBareiss             1
#define MP_CopMatrixCols                2
#define MP_CopMatrixDet                 3
#define MP_CopMatrixJacobi              4
#define MP_CopMatrixRows                6
#define MP_CopMatrixTrace               7
#define MP_CopMatrixTranspose           8
#define MP_CopMatrixKoszul              9
#define MP_CopMatrixMinor               10
#define MP_CopMatrixWedge               11

#define MP_CopMatrixDenseMatrix         12
#define MP_CopMatrixDenseVector         13

#endif /* _MP_MatrixDict_h */

#endif /* _MP_h */
