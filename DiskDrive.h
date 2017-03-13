#ifndef DISKIMAGE_H
#define DISKIMAGE_H

#include <cstdint>
#include <vector>

class DiskDrive
{
public:
    /**
     * Number of tracks on a DOS 3.3 disk.
     */
    static constexpr int NUM_TRACKS = 35;

    /**
     * Number of sectors on a DOS 3.3 disk.
     */
    static constexpr int NUM_SECTORS = 16;

    /**
     * Size of each sector in bytes.
     */
    static constexpr int SECTOR_SIZE = 256;

    /**
     * Size of a disk image in bytes (this only includes the data fields).
     */
    static constexpr int DISK_SIZE = NUM_TRACKS * NUM_SECTORS * SECTOR_SIZE;

    /**
     * Default volume number.
     */
    static constexpr uint8_t DEFAULT_VOLUME = 254;

public:
    DiskDrive();

    void LoadDisk(uint8_t disk[DISK_SIZE]);

    void SeekBit(uint8_t track_num);
    void SeekPrevByte(uint8_t track_num);

    void SetBit(uint8_t track_num, uint8_t data);
    uint8_t GetBit(uint8_t track_num);

    bool GetWriteProtect() const;

private:
    void encode_track(uint8_t track_num, uint8_t *data);
    void encode_sector(uint8_t track_num, uint8_t sector_num, uint8_t *data);
    void encode_gap(uint8_t track_num, int num_bytes);
    void encode_44(uint8_t track_num, uint8_t data);

private:
    /**
     * Encoded data for each track.
     */
    std::vector<uint8_t> _tracks[NUM_TRACKS];

    /**
     * True if a disk has been loaded, otherwise false.
     */
    bool _disk_loaded;

    /**
     * True if the disk is write protected, otherwise false.
     */
    bool _write_protected;

    /**
     * The bit to read/write. This is the current position of the disk in its
     * rotation.
     */
    uint32_t _cur_bit;
};

#endif // DISKIMAGE_H
