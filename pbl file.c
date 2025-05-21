#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// Product List Functions
void addProduct(const char* name, float price) {
    Product* newProduct = (Product*)malloc(sizeof(Product));
    strcpy(newProduct->name, name);
    newProduct->price = price;
    newProduct->next = productList;
    productList = newProduct;
    printf("Product '%s' with price %.2f added.\n", name, price);
}

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

// Linked List Functions
void addToCart(CartNode** head, const char* product) {
    CartNode* newNode = (CartNode*)malloc(sizeof(CartNode));
    strcpy(newNode->product, product);
    newNode->next = *head;
    *head = newNode;
    printf("Added to cart: %s\n", product);
}

void viewCart(CartNode* head) {
    printf("Your Cart:\n");
    fflush(stdout); // Ensure output is shown immediately
    if (!head) {
        printf("Cart is empty.\n");
        return;
    }
    while (head) {
        printf("- %s\n", head->product);
        head = head->next;
    }
}

// Tree Functions (Auto Suggestion)
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

// Queue Functions (Order Processing)
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

// Heap Functions (Highest Selling Product)
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

void showTopSelling() {
    if (heapSize == 0) {
        printf("No sales data.\n");
        return;
    }
    printf("Top selling product: %s (%d sales)\n", heap[0].product, heap[0].sales);
}

// User Interface
void userMenu(CartNode** cart, TreeNode* suggestionRoot, OrderNode** orderFront, OrderNode** orderRear) {
    int choice;
    while (1) {
        printf("\n--- User Menu ---\n");
        printf("1. Add to Cart\n");
        printf("2. View Cart\n");
        printf("3. Auto Suggestion\n");
        printf("4. Place Order\n");
        printf("5. View Product List\n");
        printf("6. Back to Main Menu\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            char product[50];
            printf("Enter product to add to cart: ");
            fgets(product, 50, stdin);
            product[strcspn(product, "\n")] = 0;
            addToCart(cart, product);
        } else if (choice == 2) {
            viewCart(*cart);
        } else if (choice == 3) {
            char prefix[50];
            printf("Enter product prefix for suggestion: ");
            fgets(prefix, 50, stdin);
            prefix[strcspn(prefix, "\n")] = 0;
            autoSuggest(suggestionRoot, prefix);
        } else if (choice == 4) {
            char order[50];
            printf("Enter order name: ");
            fgets(order, 50, stdin);
            order[strcspn(order, "\n")] = 0;
            enqueueOrder(orderFront, orderRear, order);
        } else if (choice == 5) {
            displayProducts();
        } else if (choice == 6) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// Admin Interface
void adminMenu(TreeNode** suggestionRoot) {
    int choice;
    while (1) {
        printf("\n--- Admin Menu ---\n");
        printf("1. Add Product Suggestion\n");
        printf("2. Add Sales Data\n");
        printf("3. View Top Selling Product\n");
        printf("4. Add Product with Price\n");
        printf("5. View Product List\n");
        printf("6. Back to Main Menu\n");
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
            char product[50];
            int sales;
            printf("Enter product name: ");
            fgets(product, 50, stdin);
            product[strcspn(product, "\n")] = 0;
            printf("Enter sales count: ");
            scanf("%d", &sales);
            getchar();
            insertHeap(product, sales);
            printf("Sales data added.\n");
        } else if (choice == 3) {
            showTopSelling();
        } else if (choice == 4) {
            char name[50];
            float price;
            printf("Enter product name: ");
            fgets(name, 50, stdin);
            name[strcspn(name, "\n")] = 0;
            printf("Enter product price: ");
            scanf("%f", &price);
            getchar();
            addProduct(name, price);
        } else if (choice == 5) {
            displayProducts();
        } else if (choice == 6) {
            break;
        } else {
            printf("Invalid choice.\n");
        }
    }
}

// Order Processing (Queue) for Admin
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

// Main Menu
int main() {
    int choice;
    CartNode* cart = NULL;
    TreeNode* suggestionRoot = NULL;
    OrderNode* orderFront = NULL;
    OrderNode* orderRear = NULL;

    // Sample data for suggestion and heap
    suggestionRoot = insertSuggestion(suggestionRoot, "Laptop");
    suggestionRoot = insertSuggestion(suggestionRoot, "Lamp");
    suggestionRoot = insertSuggestion(suggestionRoot, "Phone");
    suggestionRoot = insertSuggestion(suggestionRoot, "Book");
    insertHeap("Laptop", 120);
    insertHeap("Phone", 150);
    insertHeap("Book", 80);

    while (1) {
         printf("\n====================================\n");
        printf("         SHOP CONSOLE\n");
        printf("====================================\n");
        printf("\n=== Main Menu ===\n");
        printf("1. User\n");
        printf("2. Admin\n");
        printf("3. Process Orders\n");
        printf("4. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();
        if (choice == 1) {
            userMenu(&cart, suggestionRoot, &orderFront, &orderRear);
        } else if (choice == 2) {
            adminMenu(&suggestionRoot);
        } else if (choice == 3) {
            processOrdersMenu(&orderFront, &orderRear);
        } else if (choice == 4) {
            printf("Exiting program.\n");
            break;
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
    return 0;
}