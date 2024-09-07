/*
 * xlogfilepaths.h
 *
 * File name definitions and handling macros for PostgreSQL write-ahead logs.
 *
 * Portions Copyright (c) 1996-2024, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 * src/include/access/xlogfilepaths.h
 */
#ifndef XLOG_FILEPATHS_H
#define XLOG_FILEPATHS_H

#include "access/xlogdefs.h"
#include "access/xlogfilesize.h"

/*
 * The XLog directory and files (relative to $PGDATA)
 */
#define XLOGDIR					"pg_wal"

/* control files */
#define XLOG_CONTROL_FILE		"global/pg_control"
#define BACKUP_LABEL_FILE		"backup_label"
#define BACKUP_LABEL_OLD		"backup_label.old"

/* tablespace map */
#define TABLESPACE_MAP			"tablespace_map"
#define TABLESPACE_MAP_OLD		"tablespace_map.old"

/* files to signal run mode to standby */
#define RECOVERY_SIGNAL_FILE	"recovery.signal"
#define STANDBY_SIGNAL_FILE		"standby.signal"

/* files to signal promotion to primary */
#define PROMOTE_SIGNAL_FILE		"promote"


/* Maximum length of XLog file name including possible suffix */
#define MAXFNAMELEN		64

/* Length of XLog file name */
#define XLOG_FNAME_LEN	   24


/*
 * XLOG file name handling functions
 */
static inline void
StatusFilePath(char *path, const char *xlog, const char *suffix)
{
	snprintf(path, MAXPGPATH, XLOGDIR "/archive_status/%s%s", xlog, suffix);
}

static inline bool
IsTLHistoryFileName(const char *fname)
{
	return (strlen(fname) == 8 + strlen(".history") &&
			strspn(fname, "0123456789ABCDEF") == 8 &&
			strcmp(fname + 8, ".history") == 0);
}

static inline void
XLogFromFileName(const char *fname, TimeLineID *tli, XLogSegNo *logSegNo, int wal_segsz_bytes)
{
	uint32		log;
	uint32		seg;

	sscanf(fname, "%08X%08X%08X", tli, &log, &seg);
	*logSegNo = (uint64) log * XLogSegmentsPerXLogId(wal_segsz_bytes) + seg;
}

static inline void
XLogFilePath(char *path, TimeLineID tli, XLogSegNo logSegNo, int wal_segsz_bytes)
{
	snprintf(path, MAXPGPATH, XLOGDIR "/%08X%08X%08X", tli,
			 (uint32) (logSegNo / XLogSegmentsPerXLogId(wal_segsz_bytes)),
			 (uint32) (logSegNo % XLogSegmentsPerXLogId(wal_segsz_bytes)));
}

/*
 * Generate a WAL segment file name.  Do not use this function in a helper
 * function allocating the result generated.
 */
static inline void
XLogFileName(char *fname, TimeLineID tli, XLogSegNo logSegNo, int wal_segsz_bytes)
{
	snprintf(fname, MAXFNAMELEN, "%08X%08X%08X", tli,
			 (uint32) (logSegNo / XLogSegmentsPerXLogId(wal_segsz_bytes)),
			 (uint32) (logSegNo % XLogSegmentsPerXLogId(wal_segsz_bytes)));
}

static inline void
XLogFileNameById(char *fname, TimeLineID tli, uint32 log, uint32 seg)
{
	snprintf(fname, MAXFNAMELEN, "%08X%08X%08X", tli, log, seg);
}

static inline bool
IsXLogFileName(const char *fname)
{
	return (strlen(fname) == XLOG_FNAME_LEN && \
			strspn(fname, "0123456789ABCDEF") == XLOG_FNAME_LEN);
}

/*
 * XLOG segment with .partial suffix.  Used by pg_receivewal and at end of
 * archive recovery, when we want to archive a WAL segment but it might not
 * be complete yet.
 */
static inline bool
IsPartialXLogFileName(const char *fname)
{
	return (strlen(fname) == XLOG_FNAME_LEN + strlen(".partial") &&
			strspn(fname, "0123456789ABCDEF") == XLOG_FNAME_LEN &&
			strcmp(fname + XLOG_FNAME_LEN, ".partial") == 0);
}

static inline void
TLHistoryFileName(char *fname, TimeLineID tli)
{
	snprintf(fname, MAXFNAMELEN, "%08X.history", tli);
}

static inline void
TLHistoryFilePath(char *path, TimeLineID tli)
{
	snprintf(path, MAXPGPATH, XLOGDIR "/%08X.history", tli);
}

static inline void
BackupHistoryFileName(char *fname, TimeLineID tli, XLogSegNo logSegNo, XLogRecPtr startpoint, int wal_segsz_bytes)
{
	snprintf(fname, MAXFNAMELEN, "%08X%08X%08X.%08X.backup", tli,
			 (uint32) (logSegNo / XLogSegmentsPerXLogId(wal_segsz_bytes)),
			 (uint32) (logSegNo % XLogSegmentsPerXLogId(wal_segsz_bytes)),
			 (uint32) (XLogSegmentOffset(startpoint, wal_segsz_bytes)));
}

static inline bool
IsBackupHistoryFileName(const char *fname)
{
	return (strlen(fname) > XLOG_FNAME_LEN &&
			strspn(fname, "0123456789ABCDEF") == XLOG_FNAME_LEN &&
			strcmp(fname + strlen(fname) - strlen(".backup"), ".backup") == 0);
}

static inline void
BackupHistoryFilePath(char *path, TimeLineID tli, XLogSegNo logSegNo, XLogRecPtr startpoint, int wal_segsz_bytes)
{
	snprintf(path, MAXPGPATH, XLOGDIR "/%08X%08X%08X.%08X.backup", tli,
			 (uint32) (logSegNo / XLogSegmentsPerXLogId(wal_segsz_bytes)),
			 (uint32) (logSegNo % XLogSegmentsPerXLogId(wal_segsz_bytes)),
			 (uint32) (XLogSegmentOffset((startpoint), wal_segsz_bytes)));
}


#endif							/* XLOG_FILEPATHS_H */
