/*
 * This file is part of fuppes
 *
 * (c) 2013 Ulrich Völkel <u-voelkel@users.sourceforge.net>
 *
 * For the full copyright and license information, please view the COPYING
 * file that was distributed with this source code.
 */

#include <BaseTest.h>

#include <stdlib.h>
#include <string.h>

#include <Common/File.h>
#include <Common/Directory.h>

#include "ToneGenerator.h"

int main(int argc, char* argv[])
{
    Test::App app(argc, argv, Test::App::TA_NONE);


    /**
     * create the test data root dir
     */
    std::string dirname = app.getAppDir() + "testdata/";
    if(!fuppes::Directory::exists(dirname)) {
        fuppes::Directory::create(dirname);
    }



    /**
     * create a test wave file
     */
    ToneGenerator tg;
    tg.generate(60, dirname + "test.wav");

    /**
     *  create a large (10 GiB) file
     */
    std::string filename = app.getAppDir() + "testdata/largefile.bin";
    if(!fuppes::File::exists(filename)) {

        std::cout << "WARNING: this will create a 10 GiB file: " << filename << std::endl;
        int input = getchar();

        fuppes::File testfile(filename);
        testfile.open(fuppes::File::Write | fuppes::File::Truncate);

        char buffer[1024 * 1024];
        for(int i = 0; i < 10; i++) {
            memset(buffer, 48 + i, sizeof(buffer));
            for(int j = 0; j < 1024; j++) {
                testfile.write(buffer, sizeof(buffer));
            }
        }

        char end[] = "!";
        testfile.write(end, sizeof(end));
        testfile.close();
    }



}

