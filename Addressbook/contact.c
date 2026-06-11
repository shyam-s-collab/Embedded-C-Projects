#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contact.h"
#include "file.h"
#include "populate.h"

void listContacts(AddressBook *addressBook, int sortCriteria) 
{
     printf("\n%-20s %-30s %-15s\n", "Name", "Email", "Phone");
    printf("-----------------------------------------------------------\n");
     for (int i = 0; i < addressBook->contactCount; i++) {
    printf("%-20s %-30s %-15s\n", 
       addressBook->contacts[i].name, 
       addressBook->contacts[i].email, 
       addressBook->contacts[i].phone);

    } 
    
}

void initialize(AddressBook *addressBook) {
    addressBook->contactCount = 0;
    //populateAddressBook(addressBook);
    loadContactsFromFile(addressBook);
}

void saveAndExit(AddressBook *addressBook) {
    saveContactsToFile(addressBook); 
    // Save contacts to file

    exit(EXIT_SUCCESS); // Exit the program
}


void createContact(AddressBook *addressBook) {
    if (addressBook->contactCount >= MAX_CONTACTS) {
        printf("Address book is full. Cannot add more contacts.\n");
        return;
    }

    Contact newContact;
    int isDuplicate;

    printf("\n-------------------------------------------------\n");
    printf("          ENTERED INTO CREATE CONTACT\n");
    printf("-------------------------------------------------\n");

    // Name input (no uniqueness validation here)
    printf("Enter Name: ");
    scanf(" %[^\n]", newContact.name);

    // Phone number input and uniqueness check
    while (1) {
        isDuplicate = 0;
        printf("Enter Phone Number (10 digits): ");
        scanf(" %s", newContact.phone);

        if (strlen(newContact.phone) != 10) {
            printf("Phone number must be exactly 10 digits.\n");
            continue;
        }

        for (int i = 0; i < 10; i++) {
            if (!(newContact.phone[i] >= '0' && newContact.phone[i] <= '9')) {
                printf("Phone number must contain only digits.\n");
                isDuplicate = 1;
                break;
            }
        }

        if (isDuplicate) continue;

        // Check if phone already exists
        for (int i = 0; i < addressBook->contactCount; i++) {
            if (strcmp(addressBook->contacts[i].phone, newContact.phone) == 0) {
                printf("Phone number already exists.\n");
                isDuplicate = 1;
                break;
            }
        }

        if (!isDuplicate) break;
    }

    // Email input and uniqueness check
    while (1) {
        isDuplicate = 0;
        printf("Enter Email: "); 
        scanf(" %99s", newContact.email);

        int len = strlen(newContact.email);
        if (len < 5 || strstr(newContact.email, "@") == NULL || strcmp(&newContact.email[len - 4], ".com") != 0) {
            printf("Email must contain '@', end with '.com' and be valid.\n");
            continue;
        }

        // Basic character validation for email
        int valid = 1;
        for (int i = 0; i < len; i++) {
            char ch = newContact.email[i];
            if (!((ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '@' || ch == '.' || ch == '_')) {
                valid = 0;
                break;
            }
        }
        if (!valid) {
            printf("Email must contain only lowercase letters, digits, '@', '.', or '_'.\n");
            continue;
        }

        // Check if email already exists
        for (int i = 0; i < addressBook->contactCount; i++) {
            if (strcmp(addressBook->contacts[i].email, newContact.email) == 0) {
                printf("Email already exists.\n");
                isDuplicate = 1;
                break;
            }
        }

        if (!isDuplicate) break;
    }

    // Save the new contact
    addressBook->contacts[addressBook->contactCount++] = newContact;
    printf("Contact created successfully!\n");
}


//SEARCH CONTACT FUNCTION
void searchContact(AddressBook *addressBook) 
{
    int choice;
    char search[50];
    int found = 0;
    // Displaying the search menu
    printf("\n-------------------------------------------------\n");
    printf("\n          ENTERED INTO SEARCH CONTACT\n");
    printf("\n-------------------------------------------------\n");
    printf("Search By:\n");
    printf("1. Name\n");
    printf("2. Phonenumber\n");
    printf("3. Email\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    
    //USER CHOICE OF INTEREST
    switch (choice) {
        case 1:
         // Search by Name
            printf("Enter the name you want to search: ");
            scanf(" %49[^\n]s", search);
            for (int i = 0; i < addressBook->contactCount; i++) {
                // substring is used for each contact's name with the search input
                if (strstr(addressBook->contacts[i].name, search) != NULL){
                    printf("\n%-20s%-30s%-15s\n", addressBook->contacts[i].name,addressBook->contacts[i].email,addressBook->contacts[i].phone);
                    found = 1;
                }
            }
            break;

        case 2:
        //search by phone number
            printf("Enter the phone number to search: ");
            scanf(" %49[^\n]s", search);
            for (int i = 0; i < addressBook->contactCount; i++) {
                // Comparing each contact's phone number with the search input
                if (strcmp(addressBook->contacts[i].phone, search) == 0) {
                    printf("\n%s\t%s\t%s\n", 
                           addressBook->contacts[i].name,
                           addressBook->contacts[i].email,
                           addressBook->contacts[i].phone);
                    found = 1;
                }
            }
            break;

        case 3:
        //search by email
            printf("Enter the email to search: ");
            scanf(" %49[^\n]s", search);
            for (int i = 0; i < addressBook->contactCount; i++) {
                // Comparing each contact's email with the search input
                if (strcmp(addressBook->contacts[i].email, search) == 0) {
                    printf("\n%s\t%s\t%s\n", 
                           addressBook->contacts[i].name,
                           addressBook->contacts[i].email,
                           addressBook->contacts[i].phone);
                    found = 1;
                }
            }
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    if (!found) {
        printf("\nNo contact found.\n");
    }
    printf("\n-------------------------------------------------\n");
}


//EDIT CONTACT FUNCTION
void editContact(AddressBook *addressBook) {
    int choice, editChoice;
    char search[50];
    int found = 0;
    int index = -1;
    // Displaying the edit menu
    printf("\n-------------------------------------------------\n");
    printf("\n          ENTERED INTO EDIT CONTACT\n");
    printf("\n-------------------------------------------------\n");
    printf("Search By:\n");
    printf("1. Name\n");
    printf("2. Phone number\n");
    printf("3. Email\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice) {
        case 1: {
             // Search by Name
            printf("\nEnter the name : ");
            scanf(" %49[^\n]", search);

            int matchIndices[MAX_CONTACTS];
            int matchCount = 0;
            printf("------------------------------------------------------\n");
            for (int i = 0; i < addressBook->contactCount; i++) {
                //using substring to search the name 
                if (strstr(addressBook->contacts[i].name, search) != NULL) {
                    printf("%d. %s\t%s\t%s\n", matchCount + 1,
                           addressBook->contacts[i].name,
                           addressBook->contacts[i].phone,
                           addressBook->contacts[i].email);
                    matchIndices[matchCount++] = i;
                }
            }
            

            if (matchCount == 0) {
                printf("\nNo contact matched your input.\n");
                return;
            }

            if (matchCount == 1) {
                index = matchIndices[0];
            } else {
                printf("------------------------------------------------------\n");
                printf("\n Enter the number of the contact you want to edit (1-%d): ", matchCount);
                int selection;
                scanf("%d", &selection);
                if (selection < 1 || selection > matchCount) {
                    printf("Invalid selection.\n");
                    return;
                }
                index = matchIndices[selection - 1];
            }

            found = 1;
            break;
        }

        case 2:
        // Search by phone number
            printf("Enter the phone number: ");
            scanf(" %49[^\n]", search);
            for (int i = 0; i < addressBook->contactCount; i++) {
                //using sting compare we are searching phone number
                if (strcmp(addressBook->contacts[i].phone, search) == 0) {
                    printf("%s\t%s\t%s\n", 
                           addressBook->contacts[i].name,
                           addressBook->contacts[i].phone,
                           addressBook->contacts[i].email);
                    found = 1;
                    index = i;
                    break;
                }
            }
            break;

        case 3:
        // Search by Email
            printf("Enter the email: ");
            scanf(" %49[^\n]", search);
            for (int i = 0; i < addressBook->contactCount; i++) {
                //using sting compare we are searching email
                if (strcmp(addressBook->contacts[i].email, search) == 0) {
                    printf("%s\t%s\t%s\n", 
                           addressBook->contacts[i].name,
                           addressBook->contacts[i].phone,
                           addressBook->contacts[i].email);
                    found = 1;
                    index = i;
                    break;
                }
            }
            break;

        default:
            printf("Invalid choice.\n");
            return;
    }

    if (!found) {
        printf("\nNo contact found.\n");
        return;
    }

    printf("You want to edit:\n");
    printf("1. Name\n");
    printf("2. Phone number\n");
    printf("3. Email\n");
    printf("Enter your choice: ");
    scanf("%d", &editChoice);

    switch (editChoice) {
        case 1:
        //editing name
            printf("Enter the new name: ");
            scanf(" %49[^\n]", addressBook->contacts[index].name);
            printf("Name updated successfully.\n");
            break;

        case 2:
        //editing phone number
            printf("Enter the new phone number: ");
            scanf(" %19[^\n]", addressBook->contacts[index].phone);
            printf("Phone number updated successfully.\n");
            break;

        case 3:
        //editing email
            printf("Enter the new email: ");
            scanf(" %49[^\n]", addressBook->contacts[index].email);
            printf("Email updated successfully.\n");
            break;

        default:
            printf("Invalid choice.\n");
            break;
    }

    printf("\n-------------------------------------------------\n");
}

//DELECT CONTACT FUNCTION
void deleteContact(AddressBook *addressBook) {
    int choice;
    char search[50];
    int matchIndices[MAX_CONTACTS];
    int matchCount = 0;
    char confirm;

    // Displaying the delete menu
    printf("\nENTERED INTO DELETE CONTACT\n");
    printf("------------------------------------------------------\n");
    printf("Search By:\n");
    printf("1. Name\n");
    printf("2. Phone number\n");
    printf("3. Email\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);

    printf("Enter your search term: ");
    scanf(" %49[^\n]", search);

    // Search logic
    for (int i = 0; i < addressBook->contactCount; i++) {
        int match = 0;
        if (choice == 1 && strstr(addressBook->contacts[i].name, search)) {
            match = 1;
        } else if (choice == 2 && strstr(addressBook->contacts[i].phone, search)) {
            match = 1;
        } else if (choice == 3 && strstr(addressBook->contacts[i].email, search)) {
            match = 1;
        }

        if (match) {
            matchIndices[matchCount++] = i;
        }
    }

    if (matchCount == 0) {
        printf("\nNo contact matched your input.\n");
        return;
    }

    // Display matching contacts
    printf("\nMatching Contacts:\n");
    for (int i = 0; i < matchCount; i++) {
        int idx = matchIndices[i];
        printf("%d. %s\t%s\t%s\n", i + 1,
               addressBook->contacts[idx].name,
               addressBook->contacts[idx].phone,
               addressBook->contacts[idx].email);
    }

    // Ask user which contact to delete
    int selection;
    printf("\nEnter the number of the contact you want to delete (1-%d): ", matchCount);
    scanf("%d", &selection);

    if (selection < 1 || selection > matchCount) {
        printf("Invalid selection.\n");
        return;
    }

    int index = matchIndices[selection - 1];

    printf("\nYou selected to delete:\n");
    printf("%s\t%s\t%s\n",
           addressBook->contacts[index].name,
           addressBook->contacts[index].phone,
           addressBook->contacts[index].email);

    printf("Do you want to delete this contact? (y/n): ");
    scanf(" %c", &confirm);

    if (confirm == 'y' || confirm == 'Y') {
        // Shift contacts to delete
        for (int i = index; i < addressBook->contactCount - 1; i++) {
            addressBook->contacts[i] = addressBook->contacts[i + 1];
        }
        addressBook->contactCount--;
        printf("Contact deleted successfully.\n");
    } else {
        printf("Delete operation cancelled.\n");
    }
}
