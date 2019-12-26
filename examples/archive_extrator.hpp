//#pragma once
//
//#include <archive.h>
//#include <archive_entry.h>
//
//#include "std.hpp"
//
//#define ARCHIVE_BLOCK_SIZE = 0x4000
//
//class archive_extrator
//{
//private:
//    
//public:
//
//    bool extract(std::string const& in_file, std::string const& out_dir)
//    {
//        bool result = false;
//        struct archive *arc = nullptr;
//        struct archive *ext = nullptr;
//        struct archive_entry *entry = nullptr;
//        int flags = 0;
//        int ret = 0;
//        std::string out;
//
//        /* Select which attributes we want to restore. */
//        flags = ARCHIVE_EXTRACT_TIME;
//        flags |= ARCHIVE_EXTRACT_PERM;
//        flags |= ARCHIVE_EXTRACT_ACL;
//        flags |= ARCHIVE_EXTRACT_FFLAGS;
//
//        arc = archive_read_new();
//        archive_read_support_format_all(arc);
//        archive_read_support_compression_all(arc);
//        ext = archive_write_disk_new();
//        archive_write_disk_set_options(ext, flags);
//        archive_write_disk_set_standard_lookup(ext);
//
//        try
//        {
//            if ((ret = archive_read_open_filename(arc, in_file.c_str(), ARCHIVE_BLOCK_SIZE)))
//                throw std::runtime_error("can not open file: " + in_file);
//            for (;;) 
//            {
//                ret = archive_read_next_header(arc, &entry);
//                if (out_dir.empty() == false)
//                {
//                    out = out_dir + archive_entry_pathname(entry);
//                }
//                
//                //文件尾
//                if (ret == ARCHIVE_EOF)
//                    break;
//
//                if (ret < ARCHIVE_OK)
//                    throw std::runtime_error(archive_error_string(arc));
//
//                //部分成功
//                if (ret < ARCHIVE_WARN)
//                    break;
//
//                ret = archive_write_header(ext, entry);
//                if (ret < ARCHIVE_OK)
//                    throw std::runtime_error(archive_error_string(ext));
//                else if (archive_entry_size(entry) > 0) 
//                {
//                    ret = copy_data(arc, ext);
//                    if (ret < ARCHIVE_OK)
//                        throw std::runtime_error(archive_error_string(ext));
//                    //部分成功
//                    if (ret < ARCHIVE_WARN)
//                        break;
//                }
//
//                ret = archive_write_finish_entry(ext);
//                if (ret < ARCHIVE_OK)
//                    throw std::runtime_error(archive_error_string(ext));
//                
//                //部分成功
//                if (ret < ARCHIVE_WARN)
//                    break;
//            }
//
//            result = true;
//        }
//        catch (const std::exception&)
//        {
//
//        }
//
//        if (nullptr != arc)
//        {
//            archive_read_close(arc);
//            archive_read_free(arc);
//        }
//        
//        if (nullptr != ext)
//        {
//            archive_write_close(ext);
//            archive_write_free(ext);
//        }
//
//        return result;
//    }
//
//private:
//
//    int write_data(struct archive *ar, struct archive *aw)
//    {
//        int ret = 0;
//        const void *buff = nullptr;
//        size_t size = 0;
//        la_int64_t offset = 0;
//
//        for (;;) {
//            ret = archive_read_data_block(ar, &buff, &size, &offset);
//            if (ret == ARCHIVE_EOF)
//                return (ARCHIVE_OK);
//            if (ret < ARCHIVE_OK)
//                return (ret);
//            ret = archive_write_data_block(aw, buff, size, offset);
//            if (ret < ARCHIVE_OK) 
//            {
//                return (ret);
//            }
//        }
//    }
//};