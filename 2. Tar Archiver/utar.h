// v v v  Include headers here  v v v //

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
int handle_utar(char* archive_name);
int get_next_header(int fd, hdr* block_header);
