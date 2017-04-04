/**
 * Represents a single Apple II DOS 3.3 disk image. Apple II disks are composed
 * of 35 tracks (rings) that contain 16 sectors each (chunks along the ring).
 * Each sector contains exactly 256 bytes of data. On top of that data, each
 * sector also contains addressing information and "sync" bytes. It's
 * recommended to read "Beneath Apple DOS" to get a better understanding of
 * the disk format.
 *
 * Disk images that you find online usually strip out all of the unimportant
 * information and just give you the 256 bytes of data for each sector.
 * Unfortunately, the Apple II firmware not only expects the data to be
 * encoded in a special format, but it also expects all of the addressing
 * and synchronization bytes. This class handles "re-encoding" the disk image
 * to include this data.
 *
 */
#include "DiskDrive.h"

#include <iostream>

/**
 * Constructor.
 */
DiskDrive::DiskDrive() :
    _tracks(),
    _disk_loaded(false),
    _write_protected(false),
    _cur_bit(0)
{ }

/**
 * Encodes a disk image into the correct format for use by the Apple II
 * firmware.
 *
 * @param disk The raw non-encoded disk image (probably downloaded online).
 */
void DiskDrive::LoadDisk(uint8_t disk[DISK_SIZE])
{
    if(_disk_loaded)
        UnloadDisk();

    for(int i = 0; i < NUM_TRACKS; ++i)
        encode_track(i, disk + (i * NUM_SECTORS * SECTOR_SIZE));

    _disk_loaded = true;
}

/**
 * Unload a disk from the drive.
 */
void DiskDrive::UnloadDisk()
{
    for(int i = 0; i < NUM_TRACKS; ++i)
        _tracks[i].clear();

    _cur_bit = 0;
    _disk_loaded = false;
}

/**
 * Move the drive one bit position forward.
 *
 * @param track_num The track to seek on.
 */
void DiskDrive::SeekBit(uint8_t track_num)
{
    if(_disk_loaded && track_num < NUM_TRACKS)
        _cur_bit = (_cur_bit + 1) % (_tracks[track_num].size() * 8);
}

/**
 * Seeks back to the beginning of the current byte. This is used to synchronize
 * any writes to the beginning of a byte. A real disk controller uses
 * "self-sync" bytes to keep this synchronization.
 *
 * @param track_num The track to seek on.
 */
void DiskDrive::SeekPrevByte(uint8_t track_num)
{
    if(_disk_loaded && track_num < NUM_TRACKS)
        _cur_bit = (_cur_bit / 8) * 8;
}

/**
 * Set a bit on the currently loaded disk.
 *
 * @param track_num The track to write to.
 * @param data The data to write (only the LSB is written).
 */
void DiskDrive::SetBit(uint8_t track_num, uint8_t data)
{
    if(_disk_loaded && track_num < NUM_TRACKS)
    {
        int byte_index = _cur_bit / 8;
        int bit_index = 7 - (_cur_bit % 8);

        _tracks[track_num][byte_index] &= ~(1 << bit_index);
        _tracks[track_num][byte_index] |= (data & 1) << bit_index;
    }
}

/**
 * Retrieve a bit on the currently loaded disk.
 *
 * @param track_num The track to read from.
 *
 * @return The data.
 */
uint8_t DiskDrive::GetBit(uint8_t track_num)
{
    uint8_t ret_val = 0;

    if(_disk_loaded && track_num < NUM_TRACKS)
    {
        int byte_index = _cur_bit / 8;
        int bit_index = 7 - (_cur_bit % 8);

        ret_val = (_tracks[track_num][byte_index] >> bit_index) & 1;
    }

    return ret_val;
}

/**
 * Write protect switch getter.
 *
 * @return True if write protected, false otherwise.
 */
bool DiskDrive::GetWriteProtect() const
{
    return _write_protected;
}

/**
 * Encodes a single track.
 *
 * @param track_num The track to encode.
 * @param data A pointer to the data where the track begins.
 */
void DiskDrive::encode_track(uint8_t track_num, uint8_t *data)
{
    /**
     * Sector interleaving table. By not having all of the sectors located next
     * to each other in ascending order, it gives DOS more time to process a
     * sector before having to read the next one.
     */
    constexpr uint8_t sector_trans[16] = { 0, 13, 11, 9, 7, 5, 3, 1,
                                           14, 12, 10, 8, 6, 4, 2, 15 };

    for(int i = 0; i < NUM_SECTORS; ++i)
        encode_sector(track_num, sector_trans[i], data + (i * SECTOR_SIZE));
}

/**
 * Encode a single sector. This involves adding addressing information and sync
 * bytes as well as performing "6 and 2" encoding on the data.
 *
 * This code is based off of the explodeSector() function from the apple2js
 * project located here:
 * https://github.com/whscullin/apple2js/blob/master/js/disk2.js
 *
 * @param track_num The track this sector is on.
 * @param sector_num Which sector this is.
 * @param data A pointer to the data where the track begins.
 */
void DiskDrive::encode_sector(uint8_t track_num,
                              uint8_t sector_num,
                              uint8_t *data)
{
    /**
     * Translation table used to translate 8-bit data bytes into "6 and 2"
     * disk bytes.
     */
    uint8_t trans62[64] = { 0x96, 0x97, 0x9a, 0x9b, 0x9d, 0x9e, 0x9f, 0xa6,
                            0xa7, 0xab, 0xac, 0xad, 0xae, 0xaf, 0xb2, 0xb3,
                            0xb4, 0xb5, 0xb6, 0xb7, 0xb9, 0xba, 0xbb, 0xbc,
                            0xbd, 0xbe, 0xbf, 0xcb, 0xcd, 0xce, 0xcf, 0xd3,
                            0xd6, 0xd7, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde,
                            0xdf, 0xe5, 0xe6, 0xe7, 0xe9, 0xea, 0xeb, 0xec,
                            0xed, 0xee, 0xef, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6,
                            0xf7, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff };
    /**
     * Gap 1 appears before the first sector and is 128 bytes.
     * Gap 3 appears before every other sector and is 40 bytes.
     */
    const int num_gap_bytes = (sector_num == 0) ? 128 : 40;
    encode_gap(track_num, num_gap_bytes);

    /**
     * Encode the address field.
     */
    uint8_t checksum = DEFAULT_VOLUME ^ track_num ^ sector_num;
    _tracks[track_num].push_back(0xD5);
    _tracks[track_num].push_back(0xAA);
    _tracks[track_num].push_back(0x96);
    encode_44(track_num, DEFAULT_VOLUME);
    encode_44(track_num, track_num);
    encode_44(track_num, sector_num);
    encode_44(track_num, checksum);
    _tracks[track_num].push_back(0xDE);
    _tracks[track_num].push_back(0xAA);
    _tracks[track_num].push_back(0xEB);

    /**
     * Gap 2 appears between the address and data fields for 5 bytes.
     */
    encode_gap(track_num, 6);

    /**
     * Encode the data field.
     */
    _tracks[track_num].push_back(0xD5);
    _tracks[track_num].push_back(0xAA);
    _tracks[track_num].push_back(0xAD);

    /**
     * The data is being encoded using the "6 and 2" encoding described in
     * Beneath Apple DOS. This encoding takes 256 regular bytes, and encodes it
     * into 342 "disk bytes".
     *
     * The following code converts the 8-bit bytes into 6-bit and 2-bit bytes.
     * These new bytes have to be placed in a certain order so they can be
     * translated correctly by DOS.
     */
    uint8_t nibbles[342] = { 0 };

    constexpr int val6_offset = 86;
    for(int idx2 = 85, idx6 = 257; idx6 >= 0; --idx6)
    {
        uint8_t val6 = data[idx6 % 256];
        uint8_t val2 = nibbles[idx2];

        val2 = (val2 << 1) | (val6 & 1);
        val6 >>= 1;
        val2 = (val2 << 1) | (val6 & 1);
        val6 >>= 1;

        nibbles[val6_offset + idx6] = val6;
        nibbles[idx2] = val2;

        if (--idx2 < 0)
            idx2 = 85;
    }

    uint8_t last = 0;
    for(int i = 0; i < 342; ++i)
    {
        uint8_t val = nibbles[i];
        _tracks[track_num].push_back(trans62[last ^ val]);
        last = val;
    }

    _tracks[track_num].push_back(trans62[last]);

    _tracks[track_num].push_back(0xDE);
    _tracks[track_num].push_back(0xAA);
    _tracks[track_num].push_back(0xEB);

    /**
     * Beginning of the next Gap 3.
     */
    encode_gap(track_num, 1);
}

/**
 * Encodes a sequence of gap (self-sync) bytes. These are used to synchronize
 * disk controller so it knows where the start of a byte is. It also gives the
 * firmware time to decode the data and process it. Since this is an emulator,
 * we know exactly where the start of each byte is, so we can be more liberal
 * with how we encode these gap bytes.
 *
 * Usually these are 10-bit bytes (turns out "byte" doesn't imply 8-bits) where
 * the first 8-bits are 0xFF, and the last two bits are '0'. Since we know where
 * the beginning of every byte is, we don't encode those useless zero bits.
 *
 * @param track_num The track number to add the gap bytes to.
 * @param num_bytes Number of gap bytes to encode.
 */
void DiskDrive::encode_gap(uint8_t track_num, int num_bytes)
{
    for(int i = 0; i < num_bytes; ++i)
        _tracks[track_num].push_back(0xFF);
}

/**
 * Encode a byte into two separate bytes using the "4 and 4" encoding
 * system. Essentially, the data goes from:
 *
 * D7 D6 D5 D4 D3 D2 D1 D0
 *
 * To:
 *
 * 1 D7 1 D5 1 D3 1 D1
 * 1 D6 1 D4 1 D2 1 D0
 *
 * This ensures that every byte starts with a '1' and that there's no
 * adjacent zeroes (which is a requirement of the original disk controller).
 *
 * @param track_num The track to append the encoded data to.
 * @param data The byte to encode.
 */
void DiskDrive::encode_44(uint8_t track_num, uint8_t data)
{
    _tracks[track_num].push_back((data >> 1) | 0xAA);
    _tracks[track_num].push_back(data | 0xAA);
}
