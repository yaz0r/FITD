extern "C" {
    int FitdMain(int argc, char* argv[]);
}

int FitdInit(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    FitdInit(argc, argv);
    FitdMain(argc, argv);
    return 0;
}
