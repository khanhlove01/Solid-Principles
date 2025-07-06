#include <iostream>
#include <string>
#include <unordered_map>

using namespace std;

struct Product {
    int id;
    string name;
    double price;
    int stock;
};

struct CartItem {
    int productId;
    int quantity;
};

struct Cart {
    int userId;
    vector<CartItem> items;
};

// Add 1
class IProductRepository {
public:
    virtual Product* findById(int id) = 0;
};

class InMemoryProductRepository : public IProductRepository {
private:
    unordered_map<int, Product> products;

public:
    void add(const Product& p) {
        products[p.id] = p;
    }

    Product* findById(int id) override {
        auto it = products.find(id);
        if (it != products.end()) return &it->second;
        return nullptr;
    }
};



class ICartRepository {
public:
    virtual void saveCart(const Cart& cart) = 0;
    virtual Cart* getCartByUserId(int userId) = 0;
    vector<CartItem> items;
};

class InMemoryCartRepository : public ICartRepository {
private:
    unordered_map<int, Cart> carts;
public:
    void saveCart(const Cart& cart) override {
        carts[cart.userId] = cart;
    }

    Cart* getCartByUserId(int userId) {
        auto it = carts.find(userId);
        if (it != carts.end()) return &it->second;
        return nullptr;
    }
};

//Service
class CartService {
private:
    ICartRepository& cartRepo;
    IProductRepository& productRepo;

public:
    CartService(ICartRepository& r, IProductRepository& p) : cartRepo(r), productRepo(p) {}

    void addProductToCart(int userId, int productId, int quantity) {


        Cart* cart = cartRepo.getCartByUserId(userId);
        if (!cart) {
            Cart newCart{ userId,{} };
            cart = &newCart;
        }

        bool found = false;
        for (auto& item : cart->items) {
            if (item.productId == productId) {
                item.quantity += quantity;
                found = true;
                break;
            }
        }

        if (!found) {
            cart->items.push_back({ productId, quantity });
        }

        cartRepo.saveCart(*cart);
    }

    Cart* viewCart(int userId) {
        return cartRepo.getCartByUserId(userId);
    }

    //Add 1
    /*double calculateTotalPrice(int userId) {
        Cart* cart = cartRepo.getCartByUserId(userId);
        if (!cart) return 0.0;

        double total = 0.0;
        for (const auto& item : cart->items) {
            Product* product = productRepo.findById(item.productId);
            if (product) {
                total += product->price * item.quantity;
            }
        }
        return total;
    }*/

};

//Add 2
class CartPricingService {
private:
    IProductRepository& productRepo;

public:
    CartPricingService(IProductRepository& repo) : productRepo(repo) {}

    double calculate(const Cart& cart) {
        double total = 0.0;
        for (const auto& item : cart.items) {
            Product* product = productRepo.findById(item.productId);
            if (product) {
                total += product->price * item.quantity;
            }
        }
        return total;
    }
};


class CartController {
private:
    CartService& service;

    //Add 2
    CartPricingService& pricingService;

public:
    CartController(CartService& s) : service(s) {}

    void addProduct(int userId, int productId, int quantity) {
        service.addProductToCart(userId, productId, quantity);
        std::cout << "Product added to cart.\n";
    }

    void showCart(int userId) {
        Cart* cart = service.viewCart(userId);
        if (!cart || cart->items.empty()) {
            std::cout << "Cart is empty.\n";
            return;
        }

        std::cout << "Cart for user " << userId << ":\n";
        for (const auto& item : cart->items) {
            std::cout << "- Product ID: " << item.productId
                << ", Quantity: " << item.quantity << "\n";
        }
    }

    //Add 1
    void showTotal(int userId) {
        Cart* cart = service.viewCart(userId);
        if (!cart) {
            std::cout << "Cart is empty.\n";
            return;
        }
        double total = pricingService.calculate(*cart);
        cout << "Total cart value: $" << total << "\n";
    }
};
