//========================================================================
//
// CachedFile.h
//
// Caching files support.
//
// This file is licensed under the GPLv2 or later
//
// Copyright 2009 Stefan Thomas <thomas@eload24.com>
// Copyright 2010 Hib Eris <hib@hiberis.nl>
// Copyright 2010, 2018-2020, 2022 Albert Astals Cid <aacid@kde.org>
//
//========================================================================

#ifndef CACHEDFILE_H
#define CACHEDFILE_H

#include "poppler-config.h"
#include "poppler_private_export.h"

#include "Object.h"
#include "Stream.h"

#include <vector>

//------------------------------------------------------------------------

#define CachedFileChunkSize 8192 // This should be a multiple of cachedStreamBufSize

class GooString;
class CachedFileLoader;

//------------------------------------------------------------------------
// CachedFile
//
// CachedFile gives FILE-like access to a document at a specified URI.
// In the constructor, you specify a CachedFileLoader that handles loading
// the data from the document. The CachedFile requests no more data then it
// needs from the CachedFileLoader.
//------------------------------------------------------------------------

class POPPLER_PRIVATE_EXPORT CachedFile
{

    friend class CachedFileWriter;

public:
    explicit CachedFile(CachedFileLoader *cacheLoader);

    CachedFile(const CachedFile &) = delete;
    CachedFile &operator=(const CachedFile &) = delete;

    unsigned int getLength() const { return length; }
    long int tell();
    int seek(long int offset, int origin);
    size_t read(void *ptr, size_t unitsize, size_t count);
    size_t write(const char *ptr, size_t size, size_t fromByte);
    int cache(const std::vector<ByteRange> &ranges);

    // Reference counting.
    void incRefCnt();
    void decRefCnt();

private:
    ~CachedFile();

    enum ChunkState
    {
        chunkStateNew = 0,
        chunkStateLoaded
    };

    typedef struct
    {
        ChunkState state;
        char data[CachedFileChunkSize];
    } Chunk;

    int cache(size_t offset, size_t length);

    CachedFileLoader *loader;

    size_t length;
    size_t streamPos;

    std::vector<Chunk> *chunks;

    int refCnt; // reference count
};

//------------------------------------------------------------------------
// CachedFileWriter
//
// CachedFileWriter handles sequential writes to a CachedFile.
// On construction, you specify the CachedFile and the chunks of it to which data
// should be written.
//------------------------------------------------------------------------

class POPPLER_PRIVATE_EXPORT CachedFileWriter
{

public:
    // Construct a CachedFile Writer.
    // The caller is responsible for deleting the cachedFile and chunksA.
    CachedFileWriter(CachedFile *cachedFile, std::vector<int> *chunksA);

    ~CachedFileWriter();

    // Writes size bytes from ptr to cachedFile, returns number of bytes written.
    size_t write(const char *ptr, size_t size);

private:
    CachedFile *cachedFile;
    std::vector<int> *chunks;
    std::vector<int>::iterator it;
    size_t offset;
};

//------------------------------------------------------------------------
// CachedFileLoader
//
// CachedFileLoader is an abstact class that specifies the interface for
// loadng data from an URI into a CachedFile.
//------------------------------------------------------------------------

class POPPLER_PRIVATE_EXPORT CachedFileLoader
{

public:
    CachedFileLoader() = default;
    virtual ~CachedFileLoader();

    CachedFileLoader(const CachedFileLoader &) = delete;
    CachedFileLoader &operator=(const CachedFileLoader &) = delete;

    // Initializes the file load.
    // Returns the length of the file.
    // The caller is responsible for deleting cachedFile.
    virtual size_t init(CachedFile *cachedFile) = 0;

    // Loads specified byte ranges and passes it to the writer to store them.
    // Returns 0 on success, Anything but 0 on failure.
    // The caller is responsible for deleting the writer.
    virtual int load(const std::vector<ByteRange> &ranges, CachedFileWriter *writer) = 0;
};

//------------------------------------------------------------------------

#endif
