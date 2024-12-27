#include "crypt.hpp"

/**
 * @brief 產生 AES 金鑰和 IV
 * @param key 32 bytes 的 AES 金鑰 (輸出)
 * @param iv  16 bytes 的初始向量 IV (輸出)
 */
void generateAESKeyAndIV(unsigned char *key, unsigned char *iv)
{
    if (RAND_bytes(key, 32) != 1)
    {
        cerr << "Failed to generate AES key" << endl;
        exit(EXIT_FAILURE);
    }

    if (RAND_bytes(iv, 16) != 1)
    {
        cerr << "Failed to generate AES IV" << endl;
        exit(EXIT_FAILURE);
    }
}

/**
 * @brief 使用 AES-256-CBC 加密
 * @param plain_text 明文字串
 * @param key        32 bytes 的 AES 金鑰
 * @param iv         16 bytes 的初始向量 IV
 * @return 加密後的字串 (二進位形式)
 */
vector<unsigned char> encrypt(const string &plain_text, const unsigned char *key, const unsigned char *iv)
{
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "EVP_CIPHER_CTX_new failed" << endl;
        return {};
    }

    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        cerr << "EVP_EncryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int len = static_cast<int>(plain_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> cipher_text(max_len);
    int update_len = 0, final_len = 0;

    if (EVP_EncryptUpdate(ctx, cipher_text.data(), &update_len, reinterpret_cast<const unsigned char *>(plain_text.data()), len) != 1)
    {
        cerr << "EVP_EncryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_EncryptFinal_ex(ctx, cipher_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_EncryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    // 修正 vector 長度為加密後的實際長度
    cipher_text.resize(update_len + final_len);

    // cout << "Encrypted size: " << cipher_text.size() << endl;
    return cipher_text;
}

vector<unsigned char> encrypt_file(const vector<unsigned char> &plain_text, const unsigned char *key, const unsigned char *iv)
{
    // 建立 Context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "EVP_CIPHER_CTX_new failed" << endl;
        return {};
    }

    // 初始化加密：AES-256-CBC
    if (EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1)
    {
        cerr << "EVP_EncryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 準備足夠的輸出空間：明文長度 + 區塊大小
    int len = static_cast<int>(plain_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> cipher_text(max_len);
    int update_len = 0, final_len = 0;

    // 加密 Update
    if (EVP_EncryptUpdate(ctx, cipher_text.data(), &update_len, plain_text.data(), len) != 1)
    {
        cerr << "EVP_EncryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 加密 Final
    if (EVP_EncryptFinal_ex(ctx, cipher_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_EncryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 釋放 Context
    EVP_CIPHER_CTX_free(ctx);

    // 根據實際加密長度縮小 cipher_text
    cipher_text.resize(update_len + final_len);

    return cipher_text;
}

/**
 * @brief 使用 AES-256-CBC 解密
 * @param cipher_text 加密後的字串 (二進位形式)
 * @param key         32 bytes 的 AES 金鑰
 * @param iv          16 bytes 的初始向量 IV
 * @return 解密後的明文字串
 */
string decrypt(const vector<unsigned char> &origin_cipher_text, const unsigned char *key, const unsigned char *iv)
{
    // cout << "Original Cipher Length: " << origin_cipher_text.size() << endl;

    // 確保密文長度是 16 的倍數
    if (origin_cipher_text.size() % 16 != 0)
    {
        cerr << "Cipher length is not a multiple of block size, decryption aborted." << endl;
        return {};
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "Failed to create EVP_CIPHER_CTX" << endl;
        return {};
    }

    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv) != 1)
    {
        cerr << "EVP_DecryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int len = static_cast<int>(origin_cipher_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size;

    vector<unsigned char> plain_text(max_len, 0);
    int update_len = 0, final_len = 0;

    if (EVP_DecryptUpdate(ctx, plain_text.data(), &update_len, origin_cipher_text.data(), len) != 1)
    {
        cerr << "EVP_DecryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    if (EVP_DecryptFinal_ex(ctx, plain_text.data() + update_len, &final_len) != 1)
    {
        cerr << "EVP_DecryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    EVP_CIPHER_CTX_free(ctx);

    plain_text.resize(update_len + final_len);

    // cout << "Decrypted: " << plain_text.data() << endl;
    // cout << "Decrypted size: " << plain_text.size() << endl;
    return string(plain_text.begin(), plain_text.end());
}

vector<unsigned char> decrypt_file(const vector<unsigned char> &origin_cipher_text, const unsigned char *key, const unsigned char *iv)
{
    // 簡單檢查：若密文長度非 16 的倍數，可能代表不是標準的塊狀大小
    if (origin_cipher_text.size() % 16 != 0)
    {
        cerr << "Cipher length is not a multiple of block size, decryption aborted." << endl;
        return {};
    }

    // 建立 EVP 解密 Context
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
    {
        cerr << "Failed to create EVP_CIPHER_CTX" << endl;
        return {};
    }

    // 初始化解密：AES-256-CBC
    if (EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr, key, iv) != 1)
    {
        cerr << "EVP_DecryptInit_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    int len = static_cast<int>(origin_cipher_text.size());
    int block_size = EVP_CIPHER_block_size(EVP_aes_256_cbc());
    int max_len = len + block_size; // 預留可能的 Padding 空間

    vector<unsigned char> plain_text(max_len, 0);
    int update_len = 0, final_len = 0;

    // 解密 Update
    if (EVP_DecryptUpdate(ctx, plain_text.data(), &update_len, origin_cipher_text.data(), len) != 1)
    {
        cerr << "EVP_DecryptUpdate failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 解密 Final
    if (EVP_DecryptFinal_ex(ctx,
                            plain_text.data() + update_len,
                            &final_len) != 1)
    {
        cerr << "EVP_DecryptFinal_ex failed" << endl;
        EVP_CIPHER_CTX_free(ctx);
        return {};
    }

    // 釋放 Context
    EVP_CIPHER_CTX_free(ctx);

    // 調整 plain_text 為解密後的實際長度
    plain_text.resize(update_len + final_len);

    // 傳回解密後的二進位資料
    return plain_text;
}

string base64_encode(const unsigned char *input, int length)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO *bio = BIO_new(BIO_s_mem());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_push(b64, bio);

    BIO_write(b64, input, length);
    BIO_flush(b64);

    BUF_MEM *buffer_ptr;
    BIO_get_mem_ptr(b64, &buffer_ptr);
    string encoded(buffer_ptr->data, buffer_ptr->length);

    BIO_free_all(b64);
    return encoded;
}