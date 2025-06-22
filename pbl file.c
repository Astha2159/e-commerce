#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h> // For password masking

// Linked List Node for Cart
typedef struct CartNode {
    char product[50];
    struct CartNode* next;
} CartNode;

// Tree Node for Auto Suggestion
typedef struct TreeNode {
    char product[50];
    struct TreeNode* left;
    struct TreeNode* right;
} TreeNode;

// Queue Node for Orders
typedef struct OrderNode {
    char order[50];
    struct OrderNode* next;
} OrderNode;

// Heap for Highest Selling Product
#define MAX_PRODUCTS 100
typedef struct {
    char product[50];
    int sales;
} HeapNode;

HeapNode heap[MAX_PRODUCTS];
int heapSize = 0;

// Linked List for Product List
typedef struct Product {
    char name[50];
    float price;
    struct Product* next;
} Product;

Product* productList = NULL;

// --- Admin Credentials ---
char admin_id[50] = "";
char admin_pass[50] = "";

// --- Password Masking Function ---
void getPassword(char* pass, int maxLen) {
    int i = 0;
    char ch;
    while (1) {
        ch = _getch();
        if (ch == 13) { // Enter key
            pass[i] = '\0';
            printf("\n");
            break;
        } else if (ch == 8) { // Backspace
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < maxLen - 1 && ch >= 32 && ch <= 126) {
            pass[i++] = ch;
            printf("*");
        }
    }
}

// --- File Handling Functions ---

// Admin credentials
void saveAdminCredentials() {
    FILE *fp = fopen("adminpass.txt", "w");
    if (fp) {
        fprintf(fp, "%s\n%s\n", admin_id, admin_pass);
        fclose(fp);
    }
}

int loadAdminCredentials() {
    FILE *fp = fopen("adminpass.txt", "r");
    if (fp) {
        fgets(admin_id, 50, fp);
        admin_id[strcspn(admin_id, "\n")] = 0;
        fgets(admin_pass, 50, fp);
        admin_pass[strcspn(admin_pass, "\n")] = 0;
        fclose(fp);
        return 1;
    }
    return 0;
}

// Products
void saveProductToFile(const char* name, float price) {
    FILE *fp = fopen("product.txt", "a");
    if (fp) {
        fprintf(fp, "%s %.2f\n", name, price);
        fclose(fp);
    }
}

void loadProductsFromFile() {
    FILE *fp = fopen("product.txt", "r");
    if (fp) {
        char name[50];
        float price;
        while (fscanf(fp, "%s %f", name, &price) == 2) {
            // Add to linked list
            Product* newProduct = (Product*)malloc(sizeof(Product));
            strcpy(newProduct->name, name);
            newProduct->price = price;
            newProduct->next = productList;
            productList = newProduct;
        }
        fclose(fp);
    }
}

// Sales
void saveSalesToFile() {
    FILE *fp = fopen("sales.txt", "w");
    if (fp) {
        for (int i = 0; i < heapSize; i++) {
            fprintf(fp, "%s %d\n", heap[i].product, heap[i].sales);
        }
        fclose(fp);
    }
}

void loadSalesFromFile() {
    FILE *fp = fopen("sales.txt", "r");
    if (fp) {
        char product[50];
        int sales;
        while (fscanf(fp, "%s %d", product, &sales) == 2) {
            int found = 0;
            for (int i = 0; i < heapSize; i++) {
                if (strcmp(heap[i].product, product) == 0) {
                    heap[i].sales = sales;
                    found = 1;
                    break;
                }
            }
            if (!found) {
                int i = heapSize++;
                strcpy(heap[i].product, product);
                heap[i].sales = sales;
            }
        }
        fclose(fp);
    }
}

// --- Product List Functions ---
void addProduct(const char* name, float price) {
    Product* newProduct = (Product*)malloc(sizeof(Product));
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->next = productList;
    productList = newProduct;
    printf("Product '%s' with price %.2f .rs added.\n", name, price);
    saveProductToFile(name, price);
}

// --- Display Products ---
void displayProducts() {
    Product* temp = productList;
    printf("\n--- Product List ---\n");
    if (!temp) {
        printf("No products available.\n");
        return;
    }
    while (temp) {
        printf("Product: %s | Price: %.2f\n", temp->name, temp->price);
        temp = temp->next;
    }
}

// --- Linked List Functions ---
void addToCart(CartNode** head, const char* product) {
    CartNode* newNode = (CartNode*)malloc(sizeof(CartNode));
    strcpy(newNode->product, product);
    newNode->next = *head;
    *head = newNode;
    printf("Added to cart: %s\n", product);
}

void viewCart(CartNode* head) {
    printf("Your Cart:\n");
    if (!head) {
        printf("Cart is empty.\n");
        return;
    }
    while (head) {
        printf("- %s\n", head->product);
        head = head->next;
    }
}

void clearCart(CartNode **cart) {
    CartNode *current = *cart;
    while (current) {
        CartNode *tmp = current;
        current = current->next;
        free(tmp);
    }
    *cart = NULL;
}

// --- Tree Functions (Auto Suggestion) ---
TreeNode* insertSuggestion(TreeNode* root, const char* product) {
    if (!root) {
        TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
        strcpy(node->product, product);
        node->left = node->right = NULL;
        return node;
    }
    if (strcmp(product, root->product) < 0)
        root->left = insertSuggestion(root->left, product);
    else
        root->right = insertSuggestion(root->right, product);
    return root;
}

void autoSuggest(TreeNode* root, const char* prefix) {
    if (!root) return;
    if (strncmp(root->product, prefix, strlen(prefix)) == 0)
        printf("Suggestion: %s\n", root->product);
    autoSuggest(root->left, prefix);
    autoSuggest(root->right, prefix);
}

// --- Queue Functions (Order Processing) ---
void enqueueOrder(OrderNode** front, OrderNode** rear, const char* order) {
    OrderNode* newNode = (OrderNode*)malloc(sizeof(OrderNode));
    strcpy(newNode->order, order);
    newNode->next = NULL;
    if (*rear) (*rear)->next = newNode;
    else *front = newNode;
    *rear = newNode;
    printf("Order enqueued: %s\n", order);
}

void processOrder(OrderNode** front, OrderNode** rear) {
    if (!*front) {
        printf("No orders to process.\n");
        return;
    }
    OrderNode* temp = *front;
    printf("Processing order: %s\n", temp->order);
    *front = (*front)->next;
    if (!*front) *rear = NULL;
    free(temp);
}

// --- Heap Functions (Highest Selling Product) ---
void insertHeap(const char* product, int sales) {
    int i = heapSize++;
    strcpy(heap[i].product, product);
    heap[i].sales = sales;
    // Up-heapify
    while (i > 0 && heap[(i-1)/2].sales < heap[i].sales) {
        HeapNode temp = heap[i];
        heap[i] = heap[(i-1)/2];
        heap[(i-1)/2] = temp;
        i = (i-1)/2;
    }
}

void heapifyDown(int i) {
    int largest = i;
    int left = 2*i + 1;
    int right = 2*i + 2;
    if (left < heapSize && heap[left].sales > heap[largest].sales)
        largest = left;
    if (right < heapSize && heap[right].sales > heap[largest].sales)
        largest = right;
    if (largest != i) {
        HeapNode temp = heap[i];
        heap[i] = heap[largest];
        heap[largest] = temp;
        heapifyDown(largest);
    }
}

void updateSales(const char* product) {
    int found = 0;
    for (int i = 0; i < heapSize; i++) {
        if (strcmp(heap[i].product, product) == 0) {
            heap[i].sales++;
            found = 1;
            // Up-heapify to maintain max-heap property
            int child = i;
            while (child > 0 && heap[(child-1)/2].sales < heap[child].sales) {
                HeapNode temp = heap[child];
                heap[child] = heap[(child-1)/2];
                heap[(child-1)/2] = temp;
                child = (child-1)/2;
            }
            break;
        }
    }
    if (!found) {
        insertHeap(product, 1);
    }
    saveSalesToFile();
}

void showTopSelling() {
    if (productList == NULL) {
        return;
    }
    if (heapSize == 0) {
        printf("No sales yet. Be the first to order!\n");
        return;
    }
    printf("Top selling product: %s (%d sales)\n", heap[0].product, heap[0].sales);
}

// --- Admin Authentication ---
int adminAuth() {
    char id[50], pass[50];
    int registered = loadAdminCredentials();
    if (!registered) {
        printf("\n--- Admin Signup ---\n");
        printf("Set Admin ID: ");
        fgets(admin_id, 50, stdin);
        admin_id[strcspn(admin_id, "\n")] = 0;
        printf("Set Admin Password: ");
        getPassword(admin_pass, 50);
        saveAdminCredentials();
        printf("Admin signup successful! Please login next time.\n");
        return 1;
    } else {
        printf("\n--- Admin Login ---\n");
        printf("Enter Admin ID: ");
        fgets(id, 50, stdin);
        id[strcspn(id, "\n")] = 0;
        printf("Enter Admin Password: ");
        getPassword(pass, 50);
        if (strcmp(id, admin_id) == 0 && strcmp(pass, admin_pass) == 0) {
            printf("Login successful!\n");
            return 1;
        } else {
            printf("Invalid credentials!\n");
            return 0;
        }
    }
}

// --- User Interface ---
void userMenu(CartNode** cart, TreeNode* suggestionRoot, OrderNode** orderFront, OrderNode** orderRear) {
    int choice;
    while (1) {
        printf("\n--- User Menu ---\n");
        printf("1. View Product List\n");
        printf("2. Auto Suggestion\n");
        printf("3. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            // Show all products
            displayProducts();
            // Show top selling product
            showTopSelling();

            // Ask for add to cart
            if (productList!= NULL) {
                char addCart;
                printf("Do you want to add a product to cart? (y/n): ");
                scanf(" %c", &addCart);
                getchar();
                if (addCart == 'y' || addCart == 'Y') {
                    char product[50];
                    printf("Enter product name to add to cart: ");
                    fgets(product, 50, stdin);
                    product[strcspn(product, "\n")] = 0;
                    addToCart(cart, product);

                    // After add to cart, ask for view cart or place order
                    int subChoice;
                    while (1) {
                        printf("\n1. View Cart\n2. Place Order\n3. Back to User Menu\nChoose: ");
                        scanf("%d", &subChoice);
                        getchar();
                        if (subChoice == 1) {
                            viewCart(*cart);
                        } else if (subChoice == 2) {
                            // Place order for all products in the cart
                            if (*cart == NULL) {
                                printf("Cart is empty. Cannot place order.\n");
                                break;
                            }
                            CartNode* temp = *cart;
                            char orderSummary[256] = "";
                            int first = 1;
                            while (temp) {
                                enqueueOrder(orderFront, orderRear, temp->product);
                                updateSales(temp->product);
                                if (!first) strcat(orderSummary, ", ");
                                strcat(orderSummary, temp->product);
                                first = 0;
                                temp = temp->next;
                            }
                            printf("Order placed for: %s\n", orderSummary);
                            clearCart(cart); // Clear cart after order
                            break;
                        } else if (subChoice == 3) {
                            break;
                        } else {
                            printf("Invalid choice.\n");
                        }
                    }
                }
            }
        } else if (choice == 2) {
            char prefix[50];
            printf("Enter product prefix for suggestion: ");
            fgets(prefix, 50, stdin);
            prefix[strcspn(prefix, "\n")] = 0;
            autoSuggest(suggestionRoot, prefix);
        } else if (choice == 3) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// --- Admin Interface ---
void adminMenu(TreeNode** suggestionRoot) {
    int choice;
    while (1) {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Product Suggestion\n");
        printf("2. View Top Selling Product\n");
        printf("3. Add Product with Price\n");
        printf("4. View Product List\n");
        printf("5. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            char product[50];
            printf("Enter product to add to suggestions: ");
            fgets(product, 50, stdin);
            product[strcspn(product, "\n")] = 0;
            *suggestionRoot = insertSuggestion(*suggestionRoot, product);
            printf("Product suggestion added.\n");
        } else if (choice == 2) {
            showTopSelling();
        } else if (choice == 3) {
            char name[50];
            float price;
            printf("Enter product name: ");
            fgets(name, 50, stdin);
            name[strcspn(name, "\n")] = 0;
            printf("Enter product price: ");
            scanf("%f", &price);
            getchar();
            addProduct(name, price);
        } else if (choice == 4) {
            displayProducts();
        } else if (choice == 5) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// --- Order Processing (Queue) for Admin ---
void processOrdersMenu(OrderNode** orderFront, OrderNode** orderRear) {
    int choice;
    while (1) {
        printf("\n--- Order Processing ---\n");
        printf("1. Process Next Order\n");
        printf("2. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            processOrder(orderFront, orderRear);
        } else if (choice == 2) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// --- Main Menu ---
int main() {
    int choice;
    CartNode* cart = NULL;
    TreeNode* suggestionRoot = NULL;
    OrderNode* orderFront = NULL;
    OrderNode* orderRear = NULL;

    // Load products and sales from files
    loadProductsFromFile();
    loadSalesFromFile();

    // Sample data for suggestion
    suggestionRoot = insertSuggestion(suggestionRoot, "Laptop");
    suggestionRoot = insertSuggestion(suggestionRoot, "Lamp");
    suggestionRoot = insertSuggestion(suggestionRoot, "Phone");
    suggestionRoot = insertSuggestion(suggestionRoot, "Book");

    while (1) {
        printf("\n====================================\n");
        printf("         SHOP CONSOLE\n");
        printf("====================================\n");
        printf("\n=== Main Menu ===\n");
        printf("1. User\n");
        printf("2. Admin\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            userMenu(&cart, suggestionRoot, &orderFront, &orderRear);
        } else if (choice == 2) {
            if (adminAuth()) {
                adminMenu(&suggestionRoot);
            }
        } else if (choice == 3) {
            printf("Exiting program.\n");
            break;
        } else{
            printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}
