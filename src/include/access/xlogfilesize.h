/*
 * xlogfilesize.h
 *
 * Size definitions and handling macros for PostgreSQL write-ahead logs.
 *
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/xlogfilesize.h
 */

#ifndef XLOG_FILESIZE_H
#define XLOG_FILESIZE_H

#include "access/xlogdefs.h"

/* wal_segment_size can range from 1MB to 1GB */
#define WalSegMinSize 1024 * 1024
#define WalSegMaxSize 1024 * 1024 * 1024

/* default number of min and max wal segments */
#define DEFAULT_MIN_WAL_SEGS 5
#define DEFAULT_MAX_WAL_SEGS 64

/*
 * These macros encapsulate knowledge about the exact layout of XLog
 * files, timeline history fils and archive-status fils.
 */

/* check that the given size is a valid wal_segment_size */
#define IsPowerOf2(x) (x > 0 && ((x) & ((x)-1)) == 0)
#define IsValidWalSegSize(size) \
	 (IsPowerOf2(size) && \
	 ((size) >= WalSegMinSize && (size) <= WalSegMaxSize))

/* Number of segments in a logical XLOG file */
#define XLogSegmentsPerXLogId(wal_segsz_bytes)	\
	(UINT64CONST(0x100000000) / (wal_segsz_bytes))

/*
 * Compute an XLogRecPtr from a segment number and offset.
 */
#define XLogSegNoOffsetToRecPtr(segno, offset, wal_segsz_bytes, dest) \
		(dest) = (segno) * (wal_segsz_bytes) + (offset)
/*
 * Compute a segment number from an XLogRecPtr.
 *
 * For XLByteToSeg, do the computation at face value.  For XLByteToPrevSeg,
 * a boundary byte is taken to be in the previous segment.  This is suitable
 * for deciding which segment to write given a pointer to a record end,
 * for example.
 */
#define XLByteToSeg(xlrp, logSegNo, wal_segsz_bytes) \
	logSegNo = (xlrp) / (wal_segsz_bytes)

#define XLByteToPrevSeg(xlrp, logSegNo, wal_segsz_bytes) \
	logSegNo = ((xlrp) - 1) / (wal_segsz_bytes)

/* Compute the in-segment offset from an XLogRecPtr. */
#define XLogSegmentOffset(xlogptr, wal_segsz_bytes)	\
	((xlogptr) & ((wal_segsz_bytes) - 1))

/*
 * Convert values of GUCs measured in megabytes to equiv. segment count.
 * Rounds down.
 */
#define XLogMBVarToSegs(mbvar, wal_segsz_bytes) \
	((mbvar) / ((wal_segsz_bytes) / (1024 * 1024)))

/*
 * Is an XLogRecPtr within a particular XLOG segment?
 *
 * For XLByteInSeg, do the computation at face value.  For XLByteInPrevSeg,
 * a boundary byte is taken to be in the previous segment.
 */
#define XLByteInSeg(xlrp, logSegNo, wal_segsz_bytes) \
	(((xlrp) / (wal_segsz_bytes)) == (logSegNo))

#define XLByteInPrevSeg(xlrp, logSegNo, wal_segsz_bytes) \
	((((xlrp) - 1) / (wal_segsz_bytes)) == (logSegNo))


#endif							/* XLOG_FILESIZE_H */
