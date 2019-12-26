#pragma once

class cipher_aes_cbc_256
{
public:
    // 采用默认填充方式
// 该函数用来处理字符数据, 虽然是分块加解密, 也不适合传入太大数据

    std::string cbc_256_string(unsigned char const * const key, unsigned char const * const iv, std::string const& data, int mode)
    {
        EVP_CIPHER_CTX *ctx = nullptr;
        EVP_CIPHER const *ec = EVP_aes_256_cbc();
        int cipher_block_size = EVP_CIPHER_block_size(ec);
        int const BLOCK_SIZE = 0x1000;
        std::string ret;
        try
        {
            ctx = EVP_CIPHER_CTX_new();
            if (nullptr == ctx)
            {
                handler_error();
                throw std::runtime_error("cipher new error");
            }
            if (EVP_CipherInit_ex(ctx, ec, nullptr, key, iv, mode) == 0)
            {
                handler_error();
                throw std::runtime_error("cipher init error");
            }

            int remain_size = data.size();
            int pos = 0;
            // 这里要多申请一个 EVP_CIPHER_block_size 大小
            std::string out_buf;
            out_buf.resize(cipher_block_size + BLOCK_SIZE);
            int outl_total = 0;
            int outl = 0;
            char in_buf[BLOCK_SIZE];
            int in_size = 0;

            while (remain_size > 0)
            {
                // 每次最大处理0x1000长度
                int cipher_size = remain_size >= BLOCK_SIZE ? BLOCK_SIZE : remain_size;
                memcpy(in_buf, data.data() + pos, cipher_size);
                in_size = remain_size;
                if (EVP_CipherUpdate(ctx, (unsigned char*)out_buf.data(), &outl, (unsigned char*)in_buf, in_size) == 0)
                {
                    handler_error();
                    throw std::runtime_error("cipher update error");
                }

                ret.append(out_buf.data(), outl);

                // 处理总长度
                outl_total += outl;
                pos += outl;
                remain_size -= in_size;
            }

            // 默认填充方式, 需要执行final
            int tmpl = 0;
            if (EVP_CipherFinal_ex(ctx, (unsigned char*)out_buf.data() + outl, &tmpl) == 0)
            {
                handler_error();
                throw std::runtime_error("cipher final error");
            }

            ret.append(out_buf.data() + outl, tmpl);

            // 处理总长度
            outl_total += tmpl;
            pos += tmpl;

        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
            ret.clear();
        }

        if (nullptr != ctx)
        {
            EVP_CIPHER_CTX_free(ctx);
        }

        return std::move(ret);
    }

    // 采用默认填充方式
    // 该函数用来处理文件数据, 将传入文件加密或解密后写入新文件中

    bool cbc_256_file(unsigned char const * const& key, unsigned char const * const& iv, std::string const& in_file_name, std::string const& out_file_name, int const& mode)
    {
        bool ret = false;
        EVP_CIPHER_CTX *ctx = nullptr;
        EVP_CIPHER const *ec = EVP_aes_256_cbc();
        int cipher_block_size = EVP_CIPHER_block_size(ec);

        std::ofstream out_file;

        int const BLOCK_SIZE = 0x4000;

        try
        {
            bfs::path in_path(in_file_name);
            if (bfs::exists(in_file_name) == false)
            {
                throw std::runtime_error(in_file_name + " file not exists");
            }

            bfs::path out_path(out_file_name);
            if (bfs::exists(out_path.parent_path()) == false)
            {
                bfs::create_directories(out_path.parent_path());
            }

            out_file.open(out_file_name, std::ios::binary | std::ios::trunc | std::ios::out);
            if (!out_file)
            {
                throw std::runtime_error(out_file_name + " create file error");
            }

            ctx = EVP_CIPHER_CTX_new();
            if (nullptr == ctx)
            {
                handler_error();
                throw std::runtime_error("cipher new error");
            }
            if (EVP_CipherInit_ex(ctx, ec, nullptr, key, iv, mode) == 0)
            {
                handler_error();
                throw std::runtime_error("cipher init error");
            }

            auto remain_size = bfs::file_size(in_path);

            std::string out_buf;
            out_buf.resize(BLOCK_SIZE + cipher_block_size);
            int pos = 0;
            int outl_total = 0;
            int outl = 0;

            while (remain_size > 0)
            {
                uint32_t block_size = (uint32_t)(remain_size >= BLOCK_SIZE ? BLOCK_SIZE : remain_size);
                bios::file_mapping map(in_path.string().c_str(), bios::read_only);
                bios::mapped_region block_region(map, bios::read_only, pos, block_size);
                int read = block_region.get_size();
                void *addr = block_region.get_address();

                if (EVP_CipherUpdate(ctx, (unsigned char*)out_buf.data(), &outl, (unsigned char*)addr, read) == 0)
                {
                    handler_error();
                    throw std::runtime_error("cipher update error");
                }

                out_file.write(out_buf.data(), outl);

                remain_size -= read;
                outl_total += outl;
                pos += read;
            }

            // 默认填充方式, 需要执行final
            int tmpl = 0;
            if (EVP_CipherFinal_ex(ctx, (unsigned char*)out_buf.data() + outl, &tmpl) == 0)
            {
                handler_error();
                throw std::runtime_error("cipher final error");
            }

            out_file.write(out_buf.data() + outl, tmpl);

            // 处理总长度
            outl_total += tmpl;
            pos += tmpl;

            ret = true;
        }
        catch (const std::exception& e)
        {
            std::cout << e.what() << std::endl;
        }

        if (nullptr != ctx)
        {
            EVP_CIPHER_CTX_free(ctx);
        }

        return ret;
    }

private:
    static int cipher_error(const char *str, size_t len, void *u)
    {
        std::cout << "evp error: " << str << std::endl;
        return 0;
    }

    void handler_error()
    {
        ERR_print_errors_cb(cipher_error, nullptr);
    }
};

//int main()
//{
//    unsigned char key[EVP_MAX_KEY_LENGTH] = "hehehahei";
//    unsigned char iv[EVP_MAX_IV_LENGTH] = { 0 };
//
//    cipher_aes_cbc_256 cbc_256;
//
//    std::string origin = "11112222333344445555666677778888";
//    std::string cipher = cbc_256.cbc_256_string(key, iv, origin, 1);
//    std::string plant = cbc_256.cbc_256_string(key, iv, cipher, 0);
//
//    if (plant == origin)
//    {
//        std::cout << "ok" << std::endl;
//    }
//
//    bool ret = cbc_256.cbc_256_file(key, iv, "D:\\NSI_DriverInstall.log", "D:\\test_cbc_256", 1);
//    ret = cbc_256.cbc_256_file(key, iv, "D:\\test_cbc_256", "D:\\test_cbc_256.log", 0);
//    return 0;
//}