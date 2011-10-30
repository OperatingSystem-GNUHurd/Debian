/*
 * From
 * -- <linux/cdrom.h>
 * General header file for linux CD-ROM drivers 
 * Copyright (C) 1992         David Giller, rafetmad@oxy.edu
 *               1994, 1995   Eberhard Mönkeberg, emoenke@gwdg.de
 *               1996         David van Leeuwen, david@tm.tno.nl
 *               1997, 1998   Erik Andersen, andersee@debian.org
 *               1998-2002    Jens Axboe, axboe@suse.de
 */
 
#ifndef	_SYS_CDROM_H
#define	_SYS_CDROM_H

#include <endian.h>

/*******************************************************
 * As of Linux 2.1.x, all Linux CD-ROM application programs will use this 
 * (and only this) include file.  It is my hope to provide Linux with
 * a uniform interface between software accessing CD-ROMs and the various 
 * device drivers that actually talk to the drives.  There may still be
 * 23 different kinds of strange CD-ROM drives, but at least there will 
 * now be one, and only one, Linux CD-ROM interface.
 *
 * Additionally, as of Linux 2.1.x, all Linux application programs 
 * should use the O_NONBLOCK option when opening a CD-ROM device 
 * for subsequent ioctl commands.  This allows for neat system errors 
 * like "No medium found" or "Wrong medium type" upon attempting to 
 * mount or play an empty slot, mount an audio disc, or play a data disc.
 * Generally, changing an application program to support O_NONBLOCK
 * is as easy as the following:
 *       -    drive = open("/dev/cdrom", O_RDONLY);
 *       +    drive = open("/dev/cdrom", O_RDONLY | O_NONBLOCK);
 * It is worth the small change.
 *
 *  Patches for many common CD programs (provided by David A. van Leeuwen)
 *  can be found at:  ftp://ftp.gwdg.de/pub/linux/cdrom/drivers/cm206/
 * 
 *******************************************************/

/* When a driver supports a certain function, but the cdrom drive we are 
 * using doesn't, we will return the error EDRIVE_CANT_DO_THIS.  We will 
 * borrow the "Operation not supported" error from the network folks to 
 * accomplish this.  Maybe someday we will get a more targeted error code, 
 * but this will do for now... */
#define EDRIVE_CANT_DO_THIS  EOPNOTSUPP

/*******************************************************
 * The CD-ROM IOCTL commands  -- these should be supported by 
 * all the various cdrom drivers.  For the CD-ROM ioctls, we 
 * will commandeer byte 0x53, or 'S'.
 *******************************************************/
#define CDROMPAUSE		_IO('S', 0x01) /* Pause Audio Operation */ 
#define CDROMRESUME		_IO('S', 0x02) /* Resume paused Audio Operation */
#define CDROMPLAYMSF		_IOW('S', 0x03, struct cdrom_msf) /* Play Audio MSF */
#define CDROMPLAYTRKIND		_IOW('S', 0x04, struct cdrom_ti) /* Play Audio Track/index */
#define CDROMREADTOCHDR		_IOWR('S', 0x05, struct cdrom_tochdr) /* Read TOC header */
#define CDROMREADTOCENTRY	_IOWR('S', 0x06, struct cdrom_tocentry) /* Read TOC entry */
#define CDROMSTOP		_IO('S', 0x07) /* Stop the cdrom drive */
#define CDROMSTART		_IO('S', 0x08) /* Start the cdrom drive */
#define CDROMEJECT		_IO('S', 0x09) /* Ejects the cdrom media */
#define CDROMVOLCTRL		_IOW('S', 0x0a, struct cdrom_volctrl) /* Control output volume */
#define CDROMSUBCHNL		_IOWR('S', 0x0b, struct cdrom_subchnl) /* Read subchannel data */
#define CDROMREADMODE2		_IOWR('S', 0x0c, union cdrom_read_mode2) /* Read CDROM mode 2 data (2336 Bytes) */
#define CDROMREADMODE1		_IOWR('S', 0x0d, union cdrom_read_mode1) /* Read CDROM mode 1 data (2048 Bytes) */
#define CDROMREADAUDIO		_IOWR('S', 0x0e, struct cdrom_read_audio)
#define CDROMEJECT_SW		_IOIW('S', 0x0f, int) /* enable(1)/disable(0) auto-ejecting */
#define CDROMMULTISESSION	_IOWR('S', 0x10, struct cdrom_multisession) /* Obtain the start-of-last-session 
                                           address of multi session disks */
#define CDROM_GET_MCN		_IOR('S', 0x11, struct cdrom_mcn) /* Obtain the "Universal Product Code" 
                                           if available */
#define CDROM_GET_UPC		CDROM_GET_MCN  /* This one is deprecated, 
                                          but here anyway for compatibility */
#define CDROMRESET		_IO('S', 0x12) /* hard-reset the drive */
#define CDROMVOLREAD		_IOR('S', 0x13, struct cdrom_volctrl) /* Get the drive's volume setting */
#define CDROMREADRAW		_IOWR('S', 0x14, union cdrom_read_raw)	/* read data in raw mode (2352 Bytes) */
/* 
 * These ioctls are used only used in aztcd.c and optcd.c
 */
#define CDROMREADCOOKED		_IOR('S', 0x15, struct cdrom_read_cooked)	/* read data in cooked mode */
#define CDROMSEEK		_IOW('S', 0x16, struct cdrom_msf)  /* seek msf address */
  
/*
 * This ioctl is only used by the scsi-cd driver.  
   It is for playing audio in logical block addressing mode.
 */
#define CDROMPLAYBLK		_IOW('S', 0x17, struct cdrom_blk)	/* (struct cdrom_blk) */

/* 
 * These ioctls are only used in optcd.c
 */
#define CDROMREADALL		_IOR('S', 0x18, struct cdrom_read_all)	/* read all 2646 bytes */

/* 
 * These ioctls are (now) only in ide-cd.c for controlling 
 * drive spindown time.  They should be implemented in the
 * Uniform driver, via generic packet commands, GPCMD_MODE_SELECT_10,
 * GPCMD_MODE_SENSE_10 and the GPMODE_POWER_PAGE...
 *  -Erik
 */
#define CDROMGETSPINDOWN        _IOR('S', 0x1d, char)
#define CDROMSETSPINDOWN        _IOW('S', 0x1e, char)

/* 
 * These ioctls are implemented through the uniform CD-ROM driver
 * They _will_ be adopted by all CD-ROM drivers, when all the CD-ROM
 * drivers are eventually ported to the uniform CD-ROM driver interface.
 */
#define CDROMCLOSETRAY		_IO('S', 0x19)	/* pendant of CDROMEJECT */
#define CDROM_SET_OPTIONS	_IOIW('S', 0x20, int)  /* Set behavior options */
#define CDROM_CLEAR_OPTIONS	_IOIW('S', 0x21, int)  /* Clear behavior options */
#define CDROM_SELECT_SPEED	_IOIW('S', 0x22, int)  /* Set the CD-ROM speed */
#define CDROM_SELECT_DISC	_IOIW('S', 0x23, int)  /* Select disc (for juke-boxes) */
#define CDROM_MEDIA_CHANGED	_IOIW('S', 0x25, int)  /* Check is media changed  */
#define CDROM_DRIVE_STATUS	_IOIW('S', 0x26, int)  /* Get tray position, etc. */
#define CDROM_DISC_STATUS	_IO('S', 0x27)  /* Get disc type, etc. */
#define CDROM_CHANGER_NSLOTS    _IO('S', 0x28)  /* Get number of slots */
#define CDROM_LOCKDOOR		_IOIW('S', 0x29, int)  /* lock or unlock door */
#define CDROM_DEBUG		_IOIW('S', 0x30, int)	/* Turn debug messages on/off */
#define CDROM_GET_CAPABILITY	_IO('S', 0x31)	/* get capabilities */

/* Note that scsi/scsi_ioctl.h also uses _IO('S', 0x82, ) - 0x5386.
 * Future CDROM ioctls should be kept below 0x537F
 */

/* This ioctl is only used by sbpcd at the moment */
#define CDROMAUDIOBUFSIZ        _IOIW('S', 0x82, int)	/* set the audio buffer size */
					/* conflict with SCSI_IOCTL_GET_IDLUN */

/* DVD-ROM Specific ioctls */
#define DVD_READ_STRUCT		_IOWR('S', 0x90, dvd_struct)  /* Read structure */
#if 0
#define DVD_WRITE_STRUCT	_IO('S', 0x91, )  /* Write structure */
#endif
#define DVD_AUTH		_IOWR('S', 0x92, dvd_authinfo)  /* Authentication */

#define CDROM_SEND_PACKET	_IOWR('S', 0x93, struct cdrom_generic_command)	/* send a packet to the drive */
#define CDROM_NEXT_WRITABLE	_IOR('S', 0x94, long)	/* get next writable block */
#define CDROM_LAST_WRITTEN	_IOR('S', 0x95, long)	/* get last block written on disc */

/*******************************************************
 * CDROM IOCTL structures
 *******************************************************/

/* Address in MSF format */
#define _IOT_cdrom_msf0 _IOT (_IOTS (char), 3, 0, 0, 0, 0)
struct cdrom_msf0		
{
	unsigned char	minute;
	unsigned char	second;
	unsigned char	frame;
};

/* Address in either MSF or logical format */
#define _IOT_cdrom_addr _IOT (_IOTS (int), 1, 0, 0, 0, 0)
union cdrom_addr		
{
	struct cdrom_msf0	msf;
	int			lba;
};

/* This struct is used by the CDROMPLAYMSF ioctl */ 
#define _IOT_cdrom_msf _IOT (_IOTS (char), 6, 0, 0, 0, 0)
struct cdrom_msf 
{
	unsigned char	cdmsf_min0;	/* start minute */
	unsigned char	cdmsf_sec0;	/* start second */
	unsigned char	cdmsf_frame0;	/* start frame */
	unsigned char	cdmsf_min1;	/* end minute */
	unsigned char	cdmsf_sec1;	/* end second */
	unsigned char	cdmsf_frame1;	/* end frame */
};

/* This struct is used by the CDROMPLAYTRKIND ioctl */
#define _IOT_cdrom_ti _IOT (_IOTS (char), 4, 0, 0, 0, 0)
struct cdrom_ti 
{
	unsigned char	cdti_trk0;	/* start track */
	unsigned char	cdti_ind0;	/* start index */
	unsigned char	cdti_trk1;	/* end track */
	unsigned char	cdti_ind1;	/* end index */
};

/* This struct is used by the CDROMREADTOCHDR ioctl */
#define _IOT_cdrom_tochdr _IOT (_IOTS (char), 2, 0, 0, 0, 0)
struct cdrom_tochdr 	
{
	unsigned char	cdth_trk0;	/* start track */
	unsigned char	cdth_trk1;	/* end track */
};

/* This struct is used by the CDROMVOLCTRL and CDROMVOLREAD ioctls */
#define _IOT_cdrom_volctrl _IOT (_IOTS (char), 4, 0, 0, 0, 0)
struct cdrom_volctrl
{
	unsigned char	channel0;
	unsigned char	channel1;
	unsigned char	channel2;
	unsigned char	channel3;
};

/* This struct is used by the CDROMSUBCHNL ioctl */
#define _IOT_cdrom_subchnl _IOT (_IOTS (char), 5, _IOTS (int), 2, 0, 0)
struct cdrom_subchnl 
{
	unsigned char	cdsc_format;
	unsigned char	cdsc_audiostatus;
	unsigned char	cdsc_adr:	4;
	unsigned char	cdsc_ctrl:	4;
	unsigned char	cdsc_trk;
	unsigned char	cdsc_ind;
	union cdrom_addr cdsc_absaddr;
	union cdrom_addr cdsc_reladdr;
};

/* This struct is used by the CDROMREADTOCENTRY ioctl */
#define _IOT_cdrom_tocentry _IOT (_IOTS (char), 3, _IOTS (int), 1, _IOTS (char), 1)
struct cdrom_tocentry 
{
	unsigned char	cdte_track;
	unsigned char	cdte_adr	:4;
	unsigned char	cdte_ctrl	:4;
	unsigned char	cdte_format;
	union cdrom_addr cdte_addr;
	unsigned char	cdte_datamode;
};

/* This struct used by the CDROMREADMODE1, and CDROMREADMODE2 ioctls */
struct cdrom_read      
{
	int	cdread_lba;
	char 	*cdread_bufaddr;
	int	cdread_buflen;
};

/* This struct is used by the CDROMREADAUDIO ioctl */
struct cdrom_read_audio
{
	union cdrom_addr addr; /* frame address */
	unsigned char addr_format;      /* CDROM_LBA or CDROM_MSF */
	int nframes;           /* number of 2352-byte-frames to read at once */
	unsigned char *buf;      /* frame buffer (size: nframes*2352 bytes) */
};

/* This struct is used with the CDROMMULTISESSION ioctl */
#define _IOT_cdrom_multisession _IOT (_IOTS (int), 1, _IOTS (char), 2, 0, 0)
struct cdrom_multisession
{
	union cdrom_addr addr; /* frame address: start-of-last-session 
	                           (not the new "frame 16"!).  Only valid
	                           if the "xa_flag" is true. */
	unsigned char xa_flag;        /* 1: "is XA disk" */
	unsigned char addr_format;    /* CDROM_LBA or CDROM_MSF */
};

/* This struct is used with the CDROM_GET_MCN ioctl.  
 * Very few audio discs actually have Universal Product Code information, 
 * which should just be the Medium Catalog Number on the box.  Also note 
 * that the way the codeis written on CD is _not_ uniform across all discs!
 */  
#define _IOT_cdrom_mcn _IOT (_IOTS (char), 14, 0, 0, 0, 0)
struct cdrom_mcn 
{
  unsigned char medium_catalog_number[14]; /* 13 ASCII digits, null-terminated */
};

/* This is used by the CDROMPLAYBLK ioctl */
#define _IOT_cdrom_blk _IOT (_IOTS (unsigned), 1, _IOTS (short), 1, 0, 0)
struct cdrom_blk 
{
	unsigned from;
	unsigned short len;
};

#define CDROM_PACKET_SIZE	12

#define CGC_DATA_UNKNOWN	0
#define CGC_DATA_WRITE		1
#define CGC_DATA_READ		2
#define CGC_DATA_NONE		3

/* for CDROM_PACKET_COMMAND ioctl */
struct cdrom_generic_command
{
	unsigned char 		cmd[CDROM_PACKET_SIZE];
	unsigned char		*buffer;
	unsigned int 		buflen;
	int			stat;
	struct request_sense	*sense;
	unsigned char		data_direction;
	int			quiet;
	int			timeout;
	void			*reserved[1];	/* unused, actually */
};

/*
 * A CD-ROM physical sector size is 2048, 2052, 2056, 2324, 2332, 2336, 
 * 2340, or 2352 bytes long.  

*         Sector types of the standard CD-ROM data formats:
 *
 * format   sector type               user data size (bytes)
 * -----------------------------------------------------------------------------
 *   1     (Red Book)    CD-DA          2352    (CD_FRAMESIZE_RAW)
 *   2     (Yellow Book) Mode1 Form1    2048    (CD_FRAMESIZE)
 *   3     (Yellow Book) Mode1 Form2    2336    (CD_FRAMESIZE_RAW0)
 *   4     (Green Book)  Mode2 Form1    2048    (CD_FRAMESIZE)
 *   5     (Green Book)  Mode2 Form2    2328    (2324+4 spare bytes)
 *
 *
 *       The layout of the standard CD-ROM data formats:
 * -----------------------------------------------------------------------------
 * - audio (red):                  | audio_sample_bytes |
 *                                 |        2352        |
 *
 * - data (yellow, mode1):         | sync - head - data - EDC - zero - ECC |
 *                                 |  12  -   4  - 2048 -  4  -   8  - 276 |
 *
 * - data (yellow, mode2):         | sync - head - data |
 *                                 |  12  -   4  - 2336 |
 *
 * - XA data (green, mode2 form1): | sync - head - sub - data - EDC - ECC |
 *                                 |  12  -   4  -  8  - 2048 -  4  - 276 |
 *
 * - XA data (green, mode2 form2): | sync - head - sub - data - Spare |
 *                                 |  12  -   4  -  8  - 2324 -  4    |
 *
 */

/* Some generally useful CD-ROM information -- mostly based on the above */
#define CD_MINS              74 /* max. minutes per CD, not really a limit */
#define CD_SECS              60 /* seconds per minute */
#define CD_FRAMES            75 /* frames per second */
#define CD_SYNC_SIZE         12 /* 12 sync bytes per raw data frame */
#define CD_MSF_OFFSET       150 /* MSF numbering offset of first frame */
#define CD_CHUNK_SIZE        24 /* lowest-level "data bytes piece" */
#define CD_NUM_OF_CHUNKS     98 /* chunks per frame */
#define CD_FRAMESIZE_SUB     96 /* subchannel data "frame" size */
#define CD_HEAD_SIZE          4 /* header (address) bytes per raw data frame */
#define CD_SUBHEAD_SIZE       8 /* subheader bytes per raw XA data frame */
#define CD_EDC_SIZE           4 /* bytes EDC per most raw data frame types */
#define CD_ZERO_SIZE          8 /* bytes zero per yellow book mode 1 frame */
#define CD_ECC_SIZE         276 /* bytes ECC per most raw data frame types */
#define CD_FRAMESIZE       2048 /* bytes per frame, "cooked" mode */
#define CD_FRAMESIZE_RAW   2352 /* bytes per frame, "raw" mode */
#define CD_FRAMESIZE_RAWER 2646 /* The maximum possible returned bytes */ 
/* most drives don't deliver everything: */
#define CD_FRAMESIZE_RAW1 (CD_FRAMESIZE_RAW-CD_SYNC_SIZE) /*2340*/
#define CD_FRAMESIZE_RAW0 (CD_FRAMESIZE_RAW-CD_SYNC_SIZE-CD_HEAD_SIZE) /*2336*/

union cdrom_read_mode1      
{
	struct cdrom_msf	msf;
	char buffer[CD_FRAMESIZE];
};

union cdrom_read_mode2      
{
	struct cdrom_msf	msf;
	char buffer[CD_FRAMESIZE_RAW0];
};

union cdrom_read_raw
{
	struct cdrom_msf	msf;
	char buffer[CD_FRAMESIZE_RAW];
};

struct cdrom_read_cooked
{
	char buffer[CD_FRAMESIZE];
};

struct cdrom_read_all
{
	char buffer[CD_FRAMESIZE_RAWER];
};

#define CD_XA_HEAD        (CD_HEAD_SIZE+CD_SUBHEAD_SIZE) /* "before data" part of raw XA frame */
#define CD_XA_TAIL        (CD_EDC_SIZE+CD_ECC_SIZE) /* "after data" part of raw XA frame */
#define CD_XA_SYNC_HEAD   (CD_SYNC_SIZE+CD_XA_HEAD) /* sync bytes + header of XA frame */

/* CD-ROM address types (cdrom_tocentry.cdte_format) */
#define	CDROM_LBA 0x01 /* "logical block": first frame is #0 */
#define	CDROM_MSF 0x02 /* "minute-second-frame": binary, not bcd here! */

/* bit to tell whether track is data or audio (cdrom_tocentry.cdte_ctrl) */
#define	CDROM_DATA_TRACK	0x04

/* The leadout track is always 0xAA, regardless of # of tracks on disc */
#define	CDROM_LEADOUT		0xAA

/* audio states (from SCSI-2, but seen with other drives, too) */
#define	CDROM_AUDIO_INVALID	0x00	/* audio status not supported */
#define	CDROM_AUDIO_PLAY	0x11	/* audio play operation in progress */
#define	CDROM_AUDIO_PAUSED	0x12	/* audio play operation paused */
#define	CDROM_AUDIO_COMPLETED	0x13	/* audio play successfully completed */
#define	CDROM_AUDIO_ERROR	0x14	/* audio play stopped due to error */
#define	CDROM_AUDIO_NO_STATUS	0x15	/* no current audio status to return */

/* capability flags used with the uniform CD-ROM driver */ 
#define CDC_CLOSE_TRAY		0x1     /* caddy systems _can't_ close */
#define CDC_OPEN_TRAY		0x2     /* but _can_ eject.  */
#define CDC_LOCK		0x4     /* disable manual eject */
#define CDC_SELECT_SPEED 	0x8     /* programmable speed */
#define CDC_SELECT_DISC		0x10    /* select disc from juke-box */
#define CDC_MULTI_SESSION 	0x20    /* read sessions>1 */
#define CDC_MCN			0x40    /* Medium Catalog Number */
#define CDC_MEDIA_CHANGED 	0x80    /* media changed */
#define CDC_PLAY_AUDIO		0x100   /* audio functions */
#define CDC_RESET               0x200   /* hard reset device */
#define CDC_DRIVE_STATUS        0x800   /* driver implements drive status */
#define CDC_GENERIC_PACKET	0x1000	/* driver implements generic packets */
#define CDC_CD_R		0x2000	/* drive is a CD-R */
#define CDC_CD_RW		0x4000	/* drive is a CD-RW */
#define CDC_DVD			0x8000	/* drive is a DVD */
#define CDC_DVD_R		0x10000	/* drive can write DVD-R */
#define CDC_DVD_RAM		0x20000	/* drive can write DVD-RAM */
#define CDC_MO_DRIVE		0x40000 /* drive is an MO device */
#define CDC_MRW			0x80000 /* drive can read MRW */
#define CDC_MRW_W		0x100000 /* drive can write MRW */
#define CDC_RAM			0x200000 /* ok to open for WRITE */

/* drive status possibilities returned by CDROM_DRIVE_STATUS ioctl */
#define CDS_NO_INFO		0	/* if not implemented */
#define CDS_NO_DISC		1
#define CDS_TRAY_OPEN		2
#define CDS_DRIVE_NOT_READY	3
#define CDS_DISC_OK		4

/* return values for the CDROM_DISC_STATUS ioctl */
/* can also return CDS_NO_[INFO|DISC], from above */
#define CDS_AUDIO		100
#define CDS_DATA_1		101
#define CDS_DATA_2		102
#define CDS_XA_2_1		103
#define CDS_XA_2_2		104
#define CDS_MIXED		105

/* User-configurable behavior options for the uniform CD-ROM driver */
#define CDO_AUTO_CLOSE		0x1     /* close tray on first open() */
#define CDO_AUTO_EJECT		0x2     /* open tray on last release() */
#define CDO_USE_FFLAGS		0x4     /* use O_NONBLOCK information on open */
#define CDO_LOCK		0x8     /* lock tray on open files */
#define CDO_CHECK_TYPE		0x10    /* check type on open for data */

/* Special codes used when specifying changer slots. */
#define CDSL_NONE       	(INT_MAX-1)
#define CDSL_CURRENT    	INT_MAX

/* For partition based multisession access. IDE can handle 64 partitions
 * per drive - SCSI CD-ROM's use minors to differentiate between the
 * various drives, so we can't do multisessions the same way there.
 * Use the -o session=x option to mount on them.
 */
#define CD_PART_MAX		64
#define CD_PART_MASK		(CD_PART_MAX - 1)

/*********************************************************************
 * Generic Packet commands, MMC commands, and such
 *********************************************************************/

 /* The generic packet command opcodes for CD/DVD Logical Units,
 * From Table 57 of the SFF8090 Ver. 3 (Mt. Fuji) draft standard. */
#define GPCMD_BLANK			    0xa1
#define GPCMD_CLOSE_TRACK		    0x5b
#define GPCMD_FLUSH_CACHE		    0x35
#define GPCMD_FORMAT_UNIT		    0x04
#define GPCMD_GET_CONFIGURATION		    0x46
#define GPCMD_GET_EVENT_STATUS_NOTIFICATION 0x4a
#define GPCMD_GET_PERFORMANCE		    0xac
#define GPCMD_INQUIRY			    0x12
#define GPCMD_LOAD_UNLOAD		    0xa6
#define GPCMD_MECHANISM_STATUS		    0xbd
#define GPCMD_MODE_SELECT_10		    0x55
#define GPCMD_MODE_SENSE_10		    0x5a
#define GPCMD_PAUSE_RESUME		    0x4b
#define GPCMD_PLAY_AUDIO_10		    0x45
#define GPCMD_PLAY_AUDIO_MSF		    0x47
#define GPCMD_PLAY_AUDIO_TI		    0x48
#define GPCMD_PLAY_CD			    0xbc
#define GPCMD_PREVENT_ALLOW_MEDIUM_REMOVAL  0x1e
#define GPCMD_READ_10			    0x28
#define GPCMD_READ_12			    0xa8
#define GPCMD_READ_BUFFER		    0x3c
#define GPCMD_READ_BUFFER_CAPACITY	    0x5c
#define GPCMD_READ_CDVD_CAPACITY	    0x25
#define GPCMD_READ_CD			    0xbe
#define GPCMD_READ_CD_MSF		    0xb9
#define GPCMD_READ_DISC_INFO		    0x51
#define GPCMD_READ_DVD_STRUCTURE	    0xad
#define GPCMD_READ_FORMAT_CAPACITIES	    0x23
#define GPCMD_READ_HEADER		    0x44
#define GPCMD_READ_TRACK_RZONE_INFO	    0x52
#define GPCMD_READ_SUBCHANNEL		    0x42
#define GPCMD_READ_TOC_PMA_ATIP		    0x43
#define GPCMD_REPAIR_RZONE_TRACK	    0x58
#define GPCMD_REPORT_KEY		    0xa4
#define GPCMD_REQUEST_SENSE		    0x03
#define GPCMD_RESERVE_RZONE_TRACK	    0x53
#define GPCMD_SEND_CUE_SHEET		    0x5d
#define GPCMD_SCAN			    0xba
#define GPCMD_SEEK			    0x2b
#define GPCMD_SEND_DVD_STRUCTURE	    0xbf
#define GPCMD_SEND_EVENT		    0xa2
#define GPCMD_SEND_KEY			    0xa3
#define GPCMD_SEND_OPC			    0x54
#define GPCMD_SET_READ_AHEAD		    0xa7
#define GPCMD_SET_STREAMING		    0xb6
#define GPCMD_START_STOP_UNIT		    0x1b
#define GPCMD_STOP_PLAY_SCAN		    0x4e
#define GPCMD_TEST_UNIT_READY		    0x00
#define GPCMD_VERIFY_10			    0x2f
#define GPCMD_WRITE_10			    0x2a
#define GPCMD_WRITE_12			    0xaa
#define GPCMD_WRITE_AND_VERIFY_10	    0x2e
#define GPCMD_WRITE_BUFFER		    0x3b
/* This is listed as optional in ATAPI 2.6, but is (curiously) 
 * missing from Mt. Fuji, Table 57.  It _is_ mentioned in Mt. Fuji
 * Table 377 as an MMC command for SCSi devices though...  Most ATAPI
 * drives support it. */
#define GPCMD_SET_SPEED			    0xbb
/* This seems to be a SCSI specific CD-ROM opcode 
 * to play data at track/index */
#define GPCMD_PLAYAUDIO_TI		    0x48
/*
 * From MS Media Status Notification Support Specification. For
 * older drives only.
 */
#define GPCMD_GET_MEDIA_STATUS		    0xda

/* Mode page codes for mode sense/set */
#define GPMODE_VENDOR_PAGE		0x00
#define GPMODE_R_W_ERROR_PAGE		0x01
#define GPMODE_WRITE_PARMS_PAGE		0x05
#define GPMODE_WCACHING_PAGE		0x08
#define GPMODE_AUDIO_CTL_PAGE		0x0e
#define GPMODE_POWER_PAGE		0x1a
#define GPMODE_FAULT_FAIL_PAGE		0x1c
#define GPMODE_TO_PROTECT_PAGE		0x1d
#define GPMODE_CAPABILITIES_PAGE	0x2a
#define GPMODE_ALL_PAGES		0x3f
/* Not in Mt. Fuji, but in ATAPI 2.6 -- deprecated now in favor
 * of MODE_SENSE_POWER_PAGE */
#define GPMODE_CDROM_PAGE		0x0d



/* DVD struct types */
#define DVD_STRUCT_PHYSICAL	0x00
#define DVD_STRUCT_COPYRIGHT	0x01
#define DVD_STRUCT_DISCKEY	0x02
#define DVD_STRUCT_BCA		0x03
#define DVD_STRUCT_MANUFACT	0x04

struct dvd_layer {
	unsigned char book_version	: 4;
	unsigned char book_type		: 4;
	unsigned char min_rate		: 4;
	unsigned char disc_size		: 4;
	unsigned char layer_type		: 4;
	unsigned char track_path		: 1;
	unsigned char nlayers		: 2;
	unsigned char track_density	: 4;
	unsigned char linear_density	: 4;
	unsigned char bca		: 1;
	unsigned int start_sector;
	unsigned int end_sector;
	unsigned int end_sector_l0;
};

#define DVD_LAYERS	4

struct dvd_physical {
	unsigned char type;
	unsigned char layer_num;
	struct dvd_layer layer[DVD_LAYERS];
};

#define _IOT_dvd_copyright _IOT (_IOTS (char), 4, 0, 0, 0, 0)
struct dvd_copyright {
	unsigned char type;

	unsigned char layer_num;
	unsigned char cpst;
	unsigned char rmi;
};

struct dvd_disckey {
	unsigned char type;

	unsigned agid		: 2;
	unsigned char value[2048];
};

struct dvd_bca {
	unsigned char type;

	int len;
	unsigned char value[188];
};

struct dvd_manufact {
	unsigned char type;

	unsigned char layer_num;
	int len;
	unsigned char value[2048];
};

typedef union {
	unsigned char type;

	struct dvd_physical	physical;
	struct dvd_copyright	copyright;
	struct dvd_disckey	disckey;
	struct dvd_bca		bca;
	struct dvd_manufact	manufact;
} dvd_struct;

/*
 * DVD authentication ioctl
 */

/* Authentication states */
#define DVD_LU_SEND_AGID	0
#define DVD_HOST_SEND_CHALLENGE	1
#define DVD_LU_SEND_KEY1	2
#define DVD_LU_SEND_CHALLENGE	3
#define DVD_HOST_SEND_KEY2	4

/* Termination states */
#define DVD_AUTH_ESTABLISHED	5
#define DVD_AUTH_FAILURE	6

/* Other functions */
#define DVD_LU_SEND_TITLE_KEY	7
#define DVD_LU_SEND_ASF		8
#define DVD_INVALIDATE_AGID	9
#define DVD_LU_SEND_RPC_STATE	10
#define DVD_HOST_SEND_RPC_STATE	11

/* State data */
typedef unsigned char dvd_key[5];		/* 40-bit value, MSB is first elem. */
typedef unsigned char dvd_challenge[10];	/* 80-bit value, MSB is first elem. */

struct dvd_lu_send_agid {
	unsigned char type;
	unsigned agid		: 2;
};

struct dvd_host_send_challenge {
	unsigned char type;
	unsigned agid		: 2;

	dvd_challenge chal;
};

struct dvd_send_key {
	unsigned char type;
	unsigned agid		: 2;

	dvd_key key;
};

struct dvd_lu_send_challenge {
	unsigned char type;
	unsigned agid		: 2;

	dvd_challenge chal;
};

#define DVD_CPM_NO_COPYRIGHT	0
#define DVD_CPM_COPYRIGHTED	1

#define DVD_CP_SEC_NONE		0
#define DVD_CP_SEC_EXIST	1

#define DVD_CGMS_UNRESTRICTED	0
#define DVD_CGMS_SINGLE		2
#define DVD_CGMS_RESTRICTED	3

struct dvd_lu_send_title_key {
	unsigned char type;
	unsigned agid		: 2;

	dvd_key title_key;
	int lba;
	unsigned cpm		: 1;
	unsigned cp_sec		: 1;
	unsigned cgms		: 2;
};

struct dvd_lu_send_asf {
	unsigned char type;
	unsigned agid		: 2;

	unsigned asf		: 1;
};

struct dvd_host_send_rpcstate {
	unsigned char type;
	unsigned char pdrc;
};

struct dvd_lu_send_rpcstate {
	unsigned char type		: 2;
	unsigned char vra		: 3;
	unsigned char ucca		: 3;
	unsigned char region_mask;
	unsigned char rpc_scheme;
};

typedef union {
	unsigned char type;

	struct dvd_lu_send_agid		lsa;
	struct dvd_host_send_challenge	hsc;
	struct dvd_send_key		lsk;
	struct dvd_lu_send_challenge	lsc;
	struct dvd_send_key		hsk;
	struct dvd_lu_send_title_key	lstk;
	struct dvd_lu_send_asf		lsasf;
	struct dvd_host_send_rpcstate	hrpcs;
	struct dvd_lu_send_rpcstate	lrpcs;
} dvd_authinfo;

struct request_sense {
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char valid		: 1;
	unsigned char error_code		: 7;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char error_code		: 7;
	unsigned char valid		: 1;
#endif
	unsigned char segment_number;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved1		: 2;
	unsigned char ili		: 1;
	unsigned char reserved2		: 1;
	unsigned char sense_key		: 4;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char sense_key		: 4;
	unsigned char reserved2		: 1;
	unsigned char ili		: 1;
	unsigned char reserved1		: 2;
#endif
	unsigned char information[4];
	unsigned char add_sense_len;
	unsigned char command_info[4];
	unsigned char asc;
	unsigned char ascq;
	unsigned char fruc;
	unsigned char sks[3];
	unsigned char asb[46];
};

/*
 * feature profile
 */
#define CDF_RWRT	0x0020	/* "Random Writable" */
#define CDF_HWDM	0x0024	/* "Hardware Defect Management" */
#define CDF_MRW 	0x0028

/*
 * media status bits
 */
#define CDM_MRW_NOTMRW			0
#define CDM_MRW_BGFORMAT_INACTIVE	1
#define CDM_MRW_BGFORMAT_ACTIVE		2
#define CDM_MRW_BGFORMAT_COMPLETE	3

/*
 * mrw address spaces
 */
#define MRW_LBA_DMA			0
#define MRW_LBA_GAA			1

/*
 * mrw mode pages (first is deprecated) -- probed at init time and
 * cdi->mrw_mode_page is set
 */
#define MRW_MODE_PC_PRE1		0x2c
#define MRW_MODE_PC			0x03

struct mrw_feature_desc {
	unsigned short feature_code;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved1		: 2;
	unsigned char feature_version	: 4;
	unsigned char persistent		: 1;
	unsigned char curr		: 1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char curr		: 1;
	unsigned char persistent		: 1;
	unsigned char feature_version	: 4;
	unsigned char reserved1		: 2;
#endif
	unsigned char add_len;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved2		: 7;
	unsigned char write		: 1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char write		: 1;
	unsigned char reserved2		: 7;
#endif
	unsigned char reserved3;
	unsigned char reserved4;
	unsigned char reserved5;
};

/* cf. mmc4r02g.pdf 5.3.10 Random Writable Feature (0020h) pg 197 of 635 */
struct rwrt_feature_desc {
	unsigned short feature_code;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved1		: 2;
	unsigned char feature_version	: 4;
	unsigned char persistent		: 1;
	unsigned char curr		: 1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char curr		: 1;
	unsigned char persistent		: 1;
	unsigned char feature_version	: 4;
	unsigned char reserved1		: 2;
#endif
	unsigned char add_len;
	unsigned int last_lba;
	unsigned int block_size;
	unsigned short blocking;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved2		: 7;
	unsigned char page_present	: 1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char page_present	: 1;
	unsigned char reserved2		: 7;
#endif
	unsigned char reserved3;
};

typedef struct {
	unsigned short disc_information_length;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved1			: 3;
        unsigned char erasable			: 1;
        unsigned char border_status		: 2;
        unsigned char disc_status		: 2;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
        unsigned char disc_status		: 2;
        unsigned char border_status		: 2;
        unsigned char erasable			: 1;
	unsigned char reserved1			: 3;
#endif
	unsigned char n_first_track;
	unsigned char n_sessions_lsb;
	unsigned char first_track_lsb;
	unsigned char last_track_lsb;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char did_v			: 1;
        unsigned char dbc_v			: 1;
        unsigned char uru			: 1;
        unsigned char reserved2			: 2;
	unsigned char dbit			: 1;
	unsigned char mrw_status			: 2;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned char mrw_status			: 2;
	unsigned char dbit			: 1;
        unsigned char reserved2			: 2;
        unsigned char uru			: 1;
        unsigned char dbc_v			: 1;
	unsigned char did_v			: 1;
#endif
	unsigned char disc_type;
	unsigned char n_sessions_msb;
	unsigned char first_track_msb;
	unsigned char last_track_msb;
	unsigned int disc_id;
	unsigned int lead_in;
	unsigned int lead_out;
	unsigned char disc_bar_code[8];
	unsigned char reserved3;
	unsigned char n_opc;
} disc_information;

typedef struct {
	unsigned short track_information_length;
	unsigned char track_lsb;
	unsigned char session_lsb;
	unsigned char reserved1;
#if __BYTE_ORDER == __BIG_ENDIAN
	unsigned char reserved2			: 2;
        unsigned char damage			: 1;
        unsigned char copy			: 1;
        unsigned char track_mode			: 4;
	unsigned char rt				: 1;
	unsigned char blank			: 1;
	unsigned char packet			: 1;
	unsigned char fp				: 1;
	unsigned char data_mode			: 4;
	unsigned char reserved3			: 6;
	unsigned char lra_v			: 1;
	unsigned char nwa_v			: 1;
#elif __BYTE_ORDER == __LITTLE_ENDIAN
        unsigned char track_mode			: 4;
        unsigned char copy			: 1;
        unsigned char damage			: 1;
	unsigned char reserved2			: 2;
	unsigned char data_mode			: 4;
	unsigned char fp				: 1;
	unsigned char packet			: 1;
	unsigned char blank			: 1;
	unsigned char rt				: 1;
	unsigned char nwa_v			: 1;
	unsigned char lra_v			: 1;
	unsigned char reserved3			: 6;
#endif
	unsigned int track_start;
	unsigned int next_writable;
	unsigned int free_blocks;
	unsigned int fixed_packet_size;
	unsigned int track_size;
	unsigned int last_rec_address;
} track_information;

struct feature_header {
	unsigned int data_len;
	unsigned char reserved1;
	unsigned char reserved2;
	unsigned short curr_profile;
};

struct mode_page_header {
	unsigned short mode_data_length;
	unsigned char medium_type;
	unsigned char reserved1;
	unsigned char reserved2;
	unsigned char reserved3;
	unsigned short desc_length;
};


#endif  /* _SYS_CDROM_H */
