#include <stdio.h>
#include "file.h"
//invoking save contacts
void saveContactsToFile(AddressBook *addressBook) {
    FILE *fptr = fopen("contacts.txt", "w");//opening the file contact.txt in write mode
    if (fptr == NULL) {
        printf("Error: Could not open file for writing.\n");
        return;
    }
    //wirting the contact count in the contact.txt
    fprintf(fptr, "%d\n", addressBook->contactCount);

    for (int i = 0; i < addressBook->contactCount; i++) {
        fprintf(fptr, "%s,%s,%s\n",
                addressBook->contacts[i].name,
                addressBook->contacts[i].email,
                addressBook->contacts[i].phone);
    }

    fclose(fptr);
}
//invoking load contact function
void loadContactsFromFile(AddressBook *addressBook) {
    FILE *fptr = fopen("contacts.txt", "r");//opening the file contact.txt in read mode
    if (fptr == NULL) {
        printf("No saved contacts found.\n");
        addressBook->contactCount = 0;
        return;
    }
    //reading the contact count in the contact.txt
    fscanf(fptr, "%d\n", &addressBook->contactCount);

    for (int i = 0; i < addressBook->contactCount; i++) {
        fscanf(fptr, "%99[^,],%99[^,],%[^\n]\n",
               addressBook->contacts[i].name,
               addressBook->contacts[i].email,
               addressBook->contacts[i].phone);
    }

    fclose(fptr);
}
