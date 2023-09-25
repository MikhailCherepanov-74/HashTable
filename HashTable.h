#include <string>
#include <iostream>
using namespace std;

static int HashFunction(const std::string& s, int table_size, const int key)
{
    int hash_result = 0;
    for (int i = 0; i< s.size(); ++i) {
        hash_result = (key * hash_result + abs(s[i])) % table_size;
    }
    hash_result = (hash_result * 2 + 1) % table_size;
    return hash_result;
}
struct HashFunction1
{
    int operator()(const std::string& s, int table_size) const
    {
        return HashFunction(s, table_size, table_size - 1); // ключи должны быть взаимопросты, используем числа <размер таблицы> плюс и минус один.
    }
};

struct HashFunction2
{
    int operator()(const std::string& s, int table_size) const
    {
        return HashFunction(s, table_size, table_size + 1);
    }
};

template <class T, class THash1 = HashFunction1, class THash2 = HashFunction2>
class HashTable
{
    static const int default_size = 8; // начальный размер нашей таблицы
    constexpr static const double rehash_size = 0.75; // коэффициент, при котором произойдет увеличение таблицы
    struct Node
    {
        T value;
        bool state; // если значение флага state = false, значит элемент массива был удален (deleted)
        Node(const T& value_) : value(value_), state(true) {}
    };

    Node** arr; // соответственно в массиве будут хранится структуры Node*
    int size; // сколько элементов у нас сейчас в массиве (без учета deleted)
    int buffer_size; // размер самого массива, сколько памяти выделено под хранение нашей таблицы
    int size_all_non_nullptr; // сколько элементов у нас сейчас в массиве (с учетом deleted)




public:
    HashTable()
    {
        buffer_size = default_size;
        size = 0;
        size_all_non_nullptr = 0;
        arr = new Node*[buffer_size];
        for (int i = 0; i < buffer_size; ++i)
            arr[i] = nullptr; // заполняем nullptr - то есть если значение отсутствует, и никто раньше по этому адресу не обращался
    }
    ~HashTable()
    {
        for (int i = 0; i < buffer_size; ++i)
            if (arr[i])
                delete arr[i];
        delete[] arr;
    }
    void Resize()
    {
        int past_buffer_size = buffer_size;
        buffer_size *= 2;
        size_all_non_nullptr = 0;
        size = 0;
        Node** arr2 = new Node * [buffer_size];
        for (int i = 0; i < buffer_size; ++i)
            arr2[i] = nullptr;
        std::swap(arr, arr2);
        for (int i = 0; i < past_buffer_size; ++i)
        {
            if (arr2[i] && arr2[i]->state)
                Add(arr2[i]->value); // добавляем элементы в новый массив
        }
        // удаление предыдущего массива
        for (int i = 0; i < past_buffer_size; ++i)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }
    void Rehash()
    {
        size_all_non_nullptr = 0;
        size = 0;
        Node** arr2 = new Node * [buffer_size];
        for (int i = 0; i < buffer_size; ++i)
            arr2[i] = nullptr;
        std::swap(arr, arr2);
        for (int i = 0; i < buffer_size; ++i)
        {
            if (arr2[i] && arr2[i]->state)
                Add(arr2[i]->value);
        }
        // удаление предыдущего массива
        for (int i = 0; i < buffer_size; ++i)
            if (arr2[i])
                delete arr2[i];
        delete[] arr2;
    }

    bool Find(const T& value/*, const THash1& hash1 = THash1(), const THash2& hash2 = THash2()*/)
    {
        THash1 hash1 = THash1();
        THash2 hash2 = THash2();
        int h1 = hash1(value, buffer_size); // значение, отвечающее за начальную позицию
        int h2 = hash2(value, buffer_size); // значение, ответственное за "шаг" по таблице
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            if (arr[h1]->value == value && arr[h1]->state)
                return true; // такой элемент есть
            h1 = (h1 + h2) % buffer_size;
            ++i; // если у нас i >=  buffer_size, значит мы уже обошли абсолютно все ячейки, именно для этого мы считаем i, иначе мы могли бы зациклиться.
        }
        return false;
    }
    bool Remove(const T& value /*const THash1& hash1 = THash1(), const THash2& hash2 = THash2()*/)
    {
        THash1 hash1 = THash1();
        THash2 hash2 = THash2();
        int h1 = hash1(value, buffer_size);
        int h2 = hash2(value, buffer_size);
        int i = 0;
        while (arr[h1] != nullptr && i < buffer_size)
        {
            if (arr[h1]->value == value && arr[h1]->state)
            {
                arr[h1]->state = false;
                --size;
                return true;
            }
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        return false;
    }
    bool Add(const T& value/*, const THash1& hash1 = THash1(),const THash2& hash2 = THash2()*/)
    {
        THash1 hash1 = THash1();
        THash2 hash2 = THash2();
        if (size + 1 > int(rehash_size * buffer_size))
            Resize();
        else if (size_all_non_nullptr > 2 * size)
            Rehash(); // происходит рехеш, так как слишком много deleted-элементов
        int h1 = hash1(value, buffer_size);
        int h2 = hash2(value, buffer_size);
        int i = 0;
        int first_deleted = -1; // запоминаем первый подходящий элемент
        while (arr[h1] != nullptr && i < buffer_size)
        {
            if (arr[h1]->value == value && arr[h1]->state)
                return false; // такой элемент уже есть, а значит его нельзя вставлять повторно
            if (!arr[h1]->state && first_deleted == -1) // находим место для нового элемента
            {
                first_deleted = h1;
                break;               // break  -  я добавил сам.
            }
            h1 = (h1 + h2) % buffer_size;
            ++i;
        }
        if (first_deleted == -1) // если не нашлось подходящего места, создаем новый Node
        {
            arr[h1] = new Node(value);
            ++size_all_non_nullptr; // так как мы заполнили один пробел, не забываем записать, что это место теперь занято
        }
        else
        {
            arr[first_deleted]->value = value;
            arr[first_deleted]->state = true;
        }
        ++size; // и в любом случае мы увеличили количество элементов
        return true;
    }
    void PrintAll(){
     //   cout<<"size_all_non_nullptr: "<<size_all_non_nullptr<<endl;
        cout<<"size: "<<size<<endl;
        for(int i=0;i<buffer_size;i++){
            if(arr[i]!= nullptr&&arr[i]->state==true){
                cout<<i<<"  "<<arr[i]->value<<endl;
            }
        }
        cout<<endl;
    };
    void Clear(){
        for (int i = 0; i < buffer_size; ++i)
            if (arr[i])
                delete arr[i];
        delete[] arr;
        buffer_size=8;
        size=0;
        size_all_non_nullptr=0;
        arr = new Node * [buffer_size];
        for (int i = 0; i < buffer_size; ++i)
            arr[i] = nullptr;
    }

};


