# fundofc

# Brief
Develop a C program that works as a compact secure disk allowing users to store compressed and password protected files in one specific directory. The user may pass a document to the program, which encrypts the documents using a master password. The encrypted document is then compressed and stored in a directory. When required the user can decompress and decrypt the files through the program. The program may be able to batch process the tasks using command line arguments and/or work interactively. It may also come with an advanced search feature that looks for a specific keyword within an encrypted and compressed file.


This is only one example, and students are encouraged to find their own data compression and/or encryption problems for this project.

Compression techniques to consider - Run Length EncodingLinks to an external site. (relatively easy, but sufficient to meet the compression requirement of the assignment),  Huffman codingLinks to an external site. (not easy to implement - only suitable for experienced programmers) and etc.
Encryption techniques to consider - Substitution ciperLinks to an external site., XOR ciperLinks to an external site. and etc.
You could even create your own compression and/or encryption algorithm. Or you could implement multiple algorithms and allow user to pick one.

NOTE: Unlike the assignment, the group project is open ended. You are free to add your own features to the program as you see fit. You will not get a template source file.

 - Possible functions required:
 - Create User - SHould generate password and directory for upload.
 - Upload file
 - Sign out
 - Login
 - Retrieve file
 - Delete file
 - delete user
 - list all files
 - Optional extended functions
 - Search files
 - Search in a Compressed files
 - edit files???
 - Manage users - change password, login etc while preserving data
 - Size of files