// v v v  Include headers here  v v v //
#define MAGIC_NUMBER 0xCAFEBABE

//       struct for convenience       //
typedef struct {
    int magic;
    int eop;
    int block_count;
    int file_size[8];
    char deleted[8];
    int file_name[8];
    int next;
} hdr;

//         Function prototypes        //
int main(int argc, char**argv);
int validate_args(int argc, char** argv);
int handle_append(char* archive_name, char** file_list);
int handle_delete(char* archive_name, char* file);
int write_data_to_archive(int fd, hdr* block_header);
int add_file_to_header(char* file_name, hdr* block_header);
int get_next_header(int fd, hdr* block_header);
