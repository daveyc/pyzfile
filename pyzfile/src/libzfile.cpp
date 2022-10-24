#include <cstdio>
#include <cerrno>
#include <cstring>

#include <nlohmann/json.hpp>

using nlohmann::json;

extern "C" {

FILE* zfile_open(const char* file_name, const char* mode) {
    FILE* fp = fopen(file_name, mode);
    return fp;
}

int zfile_close(FILE* fp) {
    return fclose(fp);
}

size_t zfile_read(void* ptr, size_t size, size_t n, FILE* stream) {
    return fread(ptr, size, n, stream);
}

size_t zfile_write(const void* ptr, size_t size, size_t n, FILE* stream) {
    return fwrite(ptr, size, n, stream);
}

const char* zfile_strerror() {
    return strerror(errno);
}

size_t zfile_info(FILE* stream, char * outbuf, size_t size) {
    fldata_t info;
    fldata(stream, 0, &info);

    std::string openmode;
    switch (info.__openmode) {
    case __TEXT:    openmode = "TEXT"; break;
    case __BINARY:  openmode = "BINARY"; break;
    case __RECORD:  openmode = "RECORD"; break;
    case __BLOCKED: openmode = "BLOCKED"; break;
    }

    std::string vsamrls;
    switch (info.__vsamRLS) {
    case __NORLS:   vsamrls = "NORLS"; break;
    case __RLS:     vsamrls = "RLS"; break;
    case __TVS:     vsamrls = "TVS"; break;
    }

    std::string vsamtype;
    switch (info.__vsamtype) {
    case __NOTVSAM:   vsamtype = "NOTVSAM"; break;
    case __ESDS:      vsamtype = "ESDS"; break;
    case __KSDS:      vsamtype = "KSDS"; break;
    case __RRDS:      vsamtype = "RRDS"; break;
    case __ESDS_PATH: vsamtype = "ESDS_PATH"; break;
    case __KSDS_PATH: vsamtype = "KSDS_PATH"; break;
    }

    std::string device;
    switch (info.__device) {
    case __DISK:       device = "DISK"; break;
    case __TERMINAL:   device = "TERMINAL"; break;
    case __TAPE:       device = "TAPE"; break;
    case __TDQ:        device = "TDQ"; break;
    case __DUMMY:      device = "DUMMY"; break;
    case __OTHER:      device = "OTHER"; break;
    case __MEMORY:     device = "MEMORY"; break;
    case __MSGFILE:    device = "MSGFILE"; break;
    case __HFS:        device = "HFS"; break;
    case __HIPERSPACE: device = "HIPERSPACE"; break;
    case __MSGRTN:     device = "MSGSRTN"; break;
    }

    std::string access_method;
    switch (info.__access_method) {
    case __AM_UNSPEC:  access_method = "UNSPEC"; break;
    case __AM_BSAM:    access_method = "BSAM"; break;
    case __AM_QSAM:    access_method = "QSAM"; break;
    }

    std::string noseek_to_seek;
    switch (info.__noseek_to_seek) {
    case __AM_BSAM_NOSWITCH:              noseek_to_seek = "NOSWITCH"; break;
    case __AM_BSAM_UPDATE:                noseek_to_seek = "UPDATE"; break;
    case __AM_BSAM_BSAMWRITE:             noseek_to_seek = "BSAMWRITE"; break;
    case __AM_BSAM_FBS_APPEND:            noseek_to_seek = "FBS_APPEND"; break;
    case __AM_BSAM_LRECLX:                noseek_to_seek = "LRECLX"; break;
    case __AM_BSAM_PARTITIONED_DIRECTORY: noseek_to_seek = "PARTITIONED_DIRECTORY"; break;
    case __AM_BSAM_PARTITIONED_INDIRECT:  noseek_to_seek = "PARTITION_INDIRECT"; break;
    }

    json j = {  // use JSON as the serialization format
        {"recfmB", info.__recfmB ? true : false},
        {"recfmF", info.__recfmF ? true : false},
        {"recfmV", info.__recfmV ? true : false},
        {"recfmU", info.__recfmU ? true : false},
        {"recfmS", info.__recfmS ? true : false},
        {"recfmBlk", info.__recfmBlk ? true : false},
        {"recfmASA", info.__recfmASA ? true : false},
        {"recfmM", info.__recfmM ? true : false},
        {"dsorgPO", info.__dsorgPO ? true : false},
        {"dsorgPDSmem", info.__dsorgPDSmem ? true : false},
        {"dsorgPDSdir", info.__dsorgPDSdir ? true : false},
        {"dsorgPDE", info.__dsorgPDSE ? true : false},
        {"dsorgPS", info.__dsorgPS ? true : false},
        {"dsorgConcat", info.__dsorgConcat ? true : false},
        {"dsorgMem", info.__dsorgMem ? true : false},
        {"dsorgHiper", info.__dsorgHiper ? true : false},
        {"dsorgTemp", info.__dsorgTemp ? true : false},
        {"dsorgVSAM", info.__dsorgVSAM ? true : false},
        {"dsorgHFS", info.__dsorgHFS ? true : false},
        {"openmode", openmode},
        {"mode", {
            {"append", info.__modeflag & __APPEND ? true : false},
            {"read", info.__modeflag & __READ ? true : false},
            {"update", info.__modeflag & __UPDATE ? true : false},
            {"write", info.__modeflag & __WRITE ? true : false},
        }},
        {"dsorgHFS", info.__dsorgHFS ? true : false},
        {"vsamRLS", vsamrls},
        {"device", device},
        {"blksize", info.__blksize},
        {"maxreclen", info.__maxreclen},
        {"vsamtype", vsamtype},
        {"vsamkeylen", info.__vsamkeylen},
        {"vsamRKP", info.__vsamRKP},
        {"access_method", access_method},
        {"noseek_to_seek", noseek_to_seek},
        {"dsname", info.__dsname},

    };
    std::string jsonstr = j.dump();
    snprintf(outbuf, size, jsonstr.c_str(), size);
    return jsonstr.size();
};

size_t zfile_amrc(FILE* stream, char * outbuf, size_t size) {

    std::string last_op;
    switch (__amrc->__last_op) {
    case __BSAM_BLDL:           last_op = "BSAM_BLDL"; break;
    case __BSAM_CLOSE:          last_op = "BSAM_CLOSE"; break;
    case __BSAM_CLOSE_T:        last_op = "BSAM_CLOSE_T"; break;
    case __BSAM_NOTE:           last_op = "BSAM_NOTE"; break;
    case __BSAM_OPEN:           last_op = "BSAM_OPEN"; break;
    case __BSAM_POINT:          last_op = "BSAM_POINT"; break;
    case __BSAM_READ:           last_op = "BSAM_READ"; break;
    case __BSAM_STOW:           last_op = "BSAM_STOW"; break;
    case __BSAM_WRITE:          last_op = "BSAM_WRITE"; break;
    case __C_CANNOT_EXTEND:     last_op = "C_CANNOT_EXTEND"; break;
    case __C_DBCS_SI_TRUNCATE:  last_op = "C_DBCS_SI_TRUNCATE"; break;
    case __C_DBCS_SO_TRUNCATE:  last_op = "C_DBCS_SO_TRUNCATE"; break;
    case __C_DBCS_TRUNCATE:     last_op = "C_DBCS_TRUNCATE"; break;
    case __C_DBCS_UNEVEN:       last_op = "C_DBCS_UNEVEN"; break;
    case __C_FCBCHECK:          last_op = "C_FCBCHECK"; break;
    case __CICS_WRITEQ_TD:      last_op = "CICS_WRITEQ_TD"; break;
    case __C_TRUNCATE:          last_op = "C_TRUNCATE"; break;
    case __HSP_CREATE:          last_op = "HSP_CREATE"; break;
    case __HSP_DELETE:          last_op = "HSP_DELETE"; break;
    case __HSP_EXTEND:          last_op = "HSP_EXTEND"; break;
    case __HSP_READ:            last_op = "HSP_READ";  break;
    case __HSP_WRITE:           last_op = "HSP_WRITE";  break;
    case __IO_CATALOG:          last_op = "IO_CATALOG"; break;
    case __IO_DEVTYPE:          last_op = "IO_DEVTYPE"; break;
    case __IO_INIT:             last_op = "IO_INIT"; break;
    case __IO_LOCATE:           last_op = "IO_LOCATE"; break;
    case __IO_OBTAIN:           last_op = "IO_LOCATE"; break;
    case __IO_RDJFCB:           last_op = "IO_RDJFCB"; break;
    case __IO_RENAME:           last_op = "IO_RENAME"; break;
    case __IO_TRKCALC:          last_op = "IO_TRKCALC"; break;
    case __IO_UNCATALOG:        last_op = "IO_UNCATALOG"; break;
    case __LFS_CLOSE:           last_op = "LFS_CLOSE"; break;
    case __LFS_FSTAT:           last_op = "LFS_FSTAT"; break;
    case __LFS_LSEEK:           last_op = "LFS_LSEEK"; break;
    case __LFS_OPEN:            last_op = "LFS_OPEN"; break;
    case __LFS_READ:            last_op = "LFS_READ"; break;
    case __LFS_STAT:            last_op = "LFS_STAT"; break;
    case __LFS_WRITE:           last_op = "LFS_WRITE"; break;
    case __OS_CLOSE:            last_op = "OS_CLOSE"; break;
    case __OS_OPEN:             last_op = "OS_OPEN"; break;
    case __QSAM_FREEPOOL:       last_op = "QSAM_FREEPOOL"; break;
    case __QSAM_GET:            last_op = "QSAM_GET"; break;
    case __QSAM_PUT:            last_op = "QSAM_PUT"; break;
    case __QSAM_TRUNC:          last_op = "QSAM_TRUNC"; break;
    case __SVC99_ALLOC:         last_op = "SVC99_ALLOC"; break;
    case __SVC99_ALLOC_NEW:     last_op = "SVC99_ALLOC_NEW"; break;
    case __SVC99_UNALLOC:       last_op = "SVC99_UNALLOC"; break;
    case __TGET_READ:           last_op = "TGET_READ"; break;
    case __TPUT_WRITE:          last_op = "TPUT_WRITE"; break;
    case __VSAM_CLOSE:          last_op = "VSAM_CLOSE"; break;
    case __VSAM_ENDREQ:         last_op = "VSAM_ENDREQ"; break;
    case __VSAM_ERASE:          last_op = "VSAM_ERASE"; break;
    case __VSAM_GENCB:          last_op = "VSAM_GENCB"; break;
    case __VSAM_GET:            last_op = "VSAM_GET"; break;
    case __VSAM_MODCB:          last_op = "VSAM_MODCB"; break;
    case __VSAM_OPEN_ESDS:      last_op = "VSAM_OPEN_ESDS"; break;
    case __VSAM_OPEN_ESDS_PATH: last_op = "VSAM_OPEN_ESDS_PATH"; break;
    case __VSAM_OPEN_FAIL:      last_op = "VSAM_OPEN_FAIL"; break;
    case __VSAM_OPEN_KSDS:      last_op = "VSAM_OPEN_KSDS"; break;
    case __VSAM_OPEN_KSDS_PATH: last_op = "VSAM_OPEN_KSDS_PATH"; break;
    case __VSAM_OPEN_RRDS:      last_op = "VSAM_OPEN_RRDS"; break;
    }

    std::string noseek_to_seek;
    switch (__amrc->__amrc_noseek_to_seek) {
    case __AM_BSAM_NOSWITCH:              noseek_to_seek = "NOSWITCH"; break;
    case __AM_BSAM_UPDATE:                noseek_to_seek = "UPDATE"; break;
    case __AM_BSAM_BSAMWRITE:             noseek_to_seek = "BSAMWRITE"; break;
    case __AM_BSAM_FBS_APPEND:            noseek_to_seek = "FBS_APPEND"; break;
    case __AM_BSAM_LRECLX:                noseek_to_seek = "LRECLX"; break;
    case __AM_BSAM_PARTITIONED_DIRECTORY: noseek_to_seek = "PARTITIONED_DIRECTORY"; break;
    case __AM_BSAM_PARTITIONED_INDIRECT:  noseek_to_seek = "PARTITION_INDIRECT"; break;
    }

    json j = {  // use JSON as the serialization format
        {"last_op", last_op},
        {"code", {
            {"error", __amrc->__code.__error},
            {"abend", {
               {"syscode", __amrc->__code.__abend.__syscode},
               {"rc", __amrc->__code.__abend.__rc},
            }},
            {"feedback", {
               {"rc", __amrc->__code.__feedback.__rc},
               {"rtncd", __amrc->__code.__feedback.__ftncd},
               {"fdbk",__amrc->__code.__feedback.__fdbk}
           }},
           {"alloc", {
               {"svc_info",  __amrc->__code.__alloc.__svc99_info},
               {"svc_error", __amrc->__code.__alloc.__svc99_error},
           }},
        }},
        {"rba", __amrc->__RBA},
        {"msg", {
            {"len", __amrc->__msg.__len},
            {"str", __amrc->__msg.__str},
            {"parmr0", __amrc->__msg.__parmr0},
            {"parmr1", __amrc->__msg.__parmr1},
            {"xrba", __amrc->__XRBA},
        }},
        {"rba", __amrc->__RBA},
        {"amrc_noseek_to_seek", noseek_to_seek},
    };
    std::string jsonstr = j.dump();
    snprintf(outbuf, size, jsonstr.c_str(), size);
    return jsonstr.size();
};

// lookup table for flocate() options
static const std::unordered_map<std::string, int> locate_options = {
        { "key_first", __KEY_FIRST },
        { "key_last", __KEY_LAST },
        { "key_eq", __KEY_EQ },
        { "key_eq_bwd", __KEY_EQ_BWD },
        { "key_ge", __KEY_GE },
        { "rba_eq", __RBA_EQ },
        { "rba_bwd", __RBA_EQ },
};

int zfile_locate(FILE* stream,void * key, size_t length, const char * options ) {
    auto option = locate_options.find(options);
    if (option == locate_options.end()) return -1;
    return flocate(stream, key, length, option->second);
}

int zfile_update(FILE* stream, void * buffer, size_t length) {
    return fupdate(buffer, length, stream);
}

int zfile_getpos(FILE* stream, fpos_t * pos ) {
    return fgetpos(stream, pos);
}

int zfile_setpos(FILE* stream, const fpos_t * pos ) {
    return fsetpos(stream, pos);
}

// lookup table for fseek() options
static const std::unordered_map<std::string, int> seek_origin = {
        { "SEEK_SET", SEEK_SET},
        { "SEEK_CUR", SEEK_CUR},
        { "SEEK_END", SEEK_END},
};

int zfile_seek(FILE* stream, long offset, const char * origin) {
    auto whence = seek_origin.find(origin);
    if (whence == seek_origin.end()) return -1;
    return fseek(stream, offset, whence->second);
}

int zfile_rewind(FILE* stream) {
    rewind(stream);
    return errno;
}

int zfile_delrec(FILE* stream) {
    return fdelrec(stream);
}

int zfile_rba(FILE* stream) {
    return __amrc->__RBA;
}

bool zfile_eof(FILE* stream) {
    return feof(stream);
}

bool zfile_error(FILE* stream) {
    return ferror(stream);
}

} // extern "C"