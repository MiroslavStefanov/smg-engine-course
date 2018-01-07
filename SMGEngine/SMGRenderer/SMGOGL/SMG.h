#pragma once

// everything went smooth
#define SMG_OK                        S_OK

// just reports no errors
#define SMG_CANCELED                  0x82000000

// generell error message
#define SMG_FAIL                      0x82000001

// specific error messages
#define SMG_CREATEAPI                 0x82000002
#define SMG_CREATECONTEXT			  0x82000003
#define SMG_CREATEBUFFER              0x82000004
#define SMG_INVALIDPARAM              0x82000005
#define SMG_INVALIDID                 0x82000006
#define SMG_BUFFERSIZE                0x82000007
#define SMG_BUFFERLOCK                0x82000008
#define SMG_NOTCOMPATIBLE             0x82000009
#define SMG_OUTOFMEMORY               0x8200000a
#define SMG_FILENOTFOUND              0x8200000b
#define SMG_INVALIDFILE               0x8200000c
#define SMG_NOSHADERSUPPORT           0x8200000d
#define SMG_FAILa               0x8200000c
#define SMG_FAILb               0x8200000d
#define SMG_FAILc               0x8200000e
#define SMG_FAILd               0x8200000f