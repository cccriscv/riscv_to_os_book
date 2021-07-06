int a,b,c;
int *p;

int main() {
    a = 3; c = 5;
    p = &a;
    printf("a=%d b=%d c=%d\n", a, b, c);
    printf("p=%p *p=%d\n", p, *p);
}
