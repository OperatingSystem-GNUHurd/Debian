#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/bio.h>
#include <ddekit/assert.h>

struct gendisk *find_disk_by_name (char *);
void dde_page_cache_add (struct page *);

struct block_device *open_block_dev (char *name, int part, fmode_t mode)
{
  struct gendisk *disk = find_disk_by_name (name);
  dev_t devid = MKDEV (disk->major, disk->first_minor + part);
  return open_by_devnum (devid, mode);
}

/* write a piece of data to a block device.
 * DATA must be in one page.
 * SECTORNR: the writing location in sectors. */
int block_dev_write (struct block_device *dev, int sectornr,
		     char *data, int count, void (*write_done (int err)))
{
  int err = 0;
  struct bio *bio;
  struct page *page;
  int i;

  void end_bio (struct bio *bio, int err)
    {
      write_done (err);
    }

  assert (count <= PAGE_SIZE);
  bio = bio_alloc (GFP_NOIO, 1);
  if (bio == NULL)
    {
      err = ENOMEM;
      goto out;
    }

  page = kmalloc (sizeof (*page), GFP_KERNEL);
  if (page == NULL)
    {
      err = ENOMEM;
      goto out;
    }

  bio->bi_sector = sectornr;
  bio->bi_bdev = dev;
  page->virtual = data;
  dde_page_cache_add (page);
  bio->bi_io_vec[0].bv_page = page;
  bio->bi_io_vec[0].bv_len = count;
  bio->bi_io_vec[0].bv_offset = (int) data & ~PAGE_MASK;

  bio->bi_vcnt = 1;
  bio->bi_idx = 0;
  bio->bi_size = count;

  bio->bi_end_io = end_bio;
  bio->bi_private = NULL;
  bio_get (bio);
  submit_bio (WRITE, bio);
  if (bio_flagged (bio, BIO_EOPNOTSUPP))
    {
      err = -EOPNOTSUPP;
      goto out;
    }
  bio_put (bio);
out:
  if (err)
    {
      if (bio)
	bio_put (bio);
      kfree (page);
    }
  return err;
}
