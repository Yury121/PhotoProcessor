This is a preliminary project designed for processing and cataloging digital photographs.

The project has two goals: the first goal is to provide a simple interface for visualizing the results of digital processing algorithms.
The second goal is to create a program for cataloging digital photographs. This program should allow one to add information about photos, recognize people, and search for photos based on specified criteria.

The second is a post-retirement goal. I have a lot of films and I want to make these digital copies, save them, annotate them and have a solution for searching photos based on some criteria. Right now I don't have enough time to implement this.
As part of this project, I am testing possible approaches and algorithms for solving this problem and trying to determine what queries are needed and how the process should be organized.

For cataloging, a simple SQL database is used, implemented using [SQLite](https://www.sqlite.org/index.html). The program saves information about the path to digital photographs and their copies, some exif data, the full exif if it exists, and thumbnails of the photographs into a database. If face recognition has been performed for a photo, the program saves thumbnails of the found faces and a link to the corresponding file to database.


The program uses [ExifTool by Phil Harvey's](https://exiftool.org/) to extract exif information.
The [OpenVino toolkit](https://docs.openvino.ai/2023.2/home.html) and some public Intel AI networks are used for facial recognition.

This progect was started on Visual Studio 2005 platform, so current code have some legacy.
I'm not sure Microsoft won't kill Windows in the next 10-20 years, so I'm trying to implement the basic algorithms in standard C++. I like [Windows@](https://www.microsoft.com/en-us/windows) and [Microsoft Visual Studio@](https://visualstudio.microsoft.com) which are used in this project.
All trademarks belong to their respective owners.

To test this project, it is need download [Sqlite dynamic library](https://www.sqlite.org/2023/sqlite-dll-win-x64-3440200.zip) and [OpenVino toolkit](https://storage.openvinotoolkit.org/repositories/openvino/packages/2023.2/windows)

 MFC and download [Sqlite dynamic library](https://www.sqlite.org/2023/sqlite-dll-win-x64-3440200.zip) and [OpenVino toolkit](https://storage.openvinotoolkit.org/repositories/openvino/packages/2023.2/windows)

To test this project you will need Visual Studio 2022 with the latest version of MCVC and ATL libraries installed, download [Sqlite Dynamic Library](https://www.sqlite.org/2023/sqlite-dll-win-x64-3440200.zip) and [OpenVino Toolkit](https://storage.openvinotoolkit.org/repositories/openvino/packages/2023.2/windows) and place the corresponding dynamic libraries in the target folder for executing the binaries.
In addition, you need to copy the directory DLL from source folder to the target.
The Win32 target platform is deprecated and not work full.
 