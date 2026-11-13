// Generates a fake payment gateway key, in the same format as a live secret key,
// to simulate the key that the provider issues after a rotation.
//
// Usage: make new-key

# include <cstdio>
# include <random>
# include <string>

int main()
{
    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    std::random_device device;
    std::uniform_int_distribution<size_t> pick(0, alphabet.size() - 1);
    std::string key = "sk_live_";

    for (int i = 0; i < 32; i++)
    {
        key += alphabet[pick(device)];
    }

    printf("%s\n", key.c_str());
    return 0;
}
