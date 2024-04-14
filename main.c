#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

FILE *fp, *outp;
int BPM;
int nth_note;
int nth_change = 0;
int nth_counter = 0;
int c;
char buf[255];
char pitch[] = "ees";
int base_2[] = {1, 2, 4, 8, 16, 32, 64};
int base_3[] = {3, 6, 12, 24, 48, 96, 192};
int curly = 0;


void skip() {
    while (c != ',') {
        c = fgetc(fp);
    }
}

void get_bpm() {
    int i = 0;
    c = fgetc(fp);
    while (c != ')') {
        buf[i++] = (char) c;
        c = fgetc(fp);
    }
    buf[i] = '\0';
    BPM = strtof(buf, NULL);
}

void get_nth_note() {
    if (nth_counter != 0) putchar(10); //temp
    nth_counter = 0;
    int i = 0;
    c = fgetc(fp);
    while (c != '}') {
        buf[i++] = (char) c;
        c = fgetc(fp);
    }
    buf[i] = '\0';
    nth_note = (int) strtol(buf, NULL, 10); //assume there are no {4} -> {4}
    nth_change = 1;
}

int is_2base(int nth) {
    size_t size = sizeof(base_2) / sizeof(int);
    for (int i = 0; i < size; ++i) {
        if (base_2[i] == nth) {
            return base_2[i];
        }
    }
    return 0;
}

int is_3base(int nth) {
    size_t size = sizeof(base_3) / sizeof(int);
    for (int i = 0; i < size; ++i) {
        if (base_3[i] == nth) {
            return base_3[i];
        }
    }
    return 0;
}

void convert() {
    if (isdigit(c)) {
        printf("%s", pitch);
        skip();
    } else if (c == ',') {
        printf("r");
    }
    if (nth_change == 1) {
        if (is_3base(nth_note)) {
            printf("%d", nth_note * 2 / 3);
        } else if (is_2base(nth_note)) {
            printf("%d", nth_note);
        }
        nth_change = 0;
    }
    putchar(' ');

    nth_counter++;
}



int main(int argc, char* argv[]) {
    fp = fopen("./simai.txt", "r");
//    outp = fopen(argv[2], "r");
    fscanf(fp, "(%d)", &BPM);
    printf("\\header {\n"
           "\ttitle = \"Untitled\"\n"
           "\tcomposer = \"Composer\"\n"
           "}\n"
           "\\score {\n"
           "\t\\new StaffGroup <<\n"
           "\t\t\\new RhythmicStaff \\relative c'' { \n"
           "\t\t\\set Staff.midiInstrument = \"standard kit\"\n"
           "\t\t\\tempo 4 = %d\n"
           "\t\t\\numericTimeSignature\n"
           "\t\t\\time 4/4\n\t\t\t", BPM);

    while (c != 'E') {
        c = fgetc(fp);
        switch (c) {
            case '(':
                get_bpm();
                printf("\\tempo 4 = %d\n", BPM);
                break;
            case '{':
                get_nth_note();
//                printf("\n%d\n", nth_note);
                break;
            case '\n':
                if (curly != 0) {
                    putchar('}');
                    curly = 0;
                }
                printf("\t\t\t");
                break;
            case ' ':
                break;
            case '\t':
                break;
            case 'E':
                // putchar('E');
                break;
            default:
                switch (nth_note) {
                    case 12:
                        if (nth_counter % 3 == 0) {
                            printf("\\times 2/3 { ");
                            curly = 1;
                        }
                        break;
                    case 24:
                        if (nth_counter % 6 == 0) {
                            printf("\\times 4/6 { ");
                            curly = 1;
                        }
                        break;
//                    case 48:
                }

                convert();

                switch (nth_note) {
                    case 12:
                        if (nth_counter % 3 == 0) {
                            printf("} ");
                            curly = 0;
                        }
                        break;
                    case 24:
                        if (nth_counter % 6 == 0) {
                            printf("} ");
                            curly = 0;
                        }
                        break;
//                    case 48:
                }

                if (nth_counter > nth_note - 1) {
                    putchar(10);
                    nth_counter = 0;
                }

                break;
        }

    }
    printf("    }\n"
           "  >>\n"
           "  \n"
           "\n"
           "\n"
           "\n"
           "  \\layout {}\n"
           "  \\midi {}\n"
           "}");


    return 0;
}
