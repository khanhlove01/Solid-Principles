#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

//Add 2
struct Product {
	int id;
	string name;
	double price;
};
//Add 2
class IProductRepository {
public:
	virtual void save(const Product& product) = 0;
	virtual Product* findById(int productId) = 0;
};

//Add 2
class InMemoryProductRepository : public IProductRepository {
private:
	unordered_map<int, Product> products;

public:
	void save(const Product& product) override {
		products[product.id] = product;
	}

	Product* findById(int productId) override {
		auto it = products.find(productId);
		if (it != products.end()) return &it->second;
		return nullptr;
	}
};



struct Order {
	int orderId;
	int userId;
	vector <int> productIds;
};

class IOrderRepository {
public:
	virtual void save(const Order& order) = 0;
	virtual Order* findById(int orderId) = 0;

	//Add 1
	virtual void remove(int orderId) = 0;
};

class InMemoryOrderRepository : public IOrderRepository {
private:
	unordered_map<int, Order> orders;

public:
	void save(const Order& order) override {
		orders[order.orderId] = order;
	}

	Order* findById(int orderId) override {
		auto it = orders.find(orderId);
		if (it != orders.end()) return &it->second;
		return nullptr;
	}

	//Add 1
	void remove(int orderId) override {
		orders.erase(orderId);
	}
};

//Service layer

class OrderFormatter {
private:
	IProductRepository& productRepo;

public:
	OrderFormatter(IProductRepository& repo) : productRepo(repo) {}

	void print(const Order& order) {
		cout << "Order " << order.orderId << " for user " << order.userId << ":\n";
		for (int productId : order.productIds) {
			Product* p = productRepo.findById(productId);
			if (p)
				cout << "- " << p->name << " ($" << p->price << ")\n";
			else
				cout << "- Unknown product ID: " << productId << "\n";
		}
	}
};

class OrderService {
private:
	IOrderRepository& orderRepo;
	int orderSequence;

	//Add 2
	IProductRepository& productRepo;

public:
	/*OrderService(IOrderRepository& repo, int orderSequence) {
		this->repo = repo;
		this->orderSequence = orderSequence;
	}*/
	OrderService(IOrderRepository& r, int seq, IProductRepository& p) : orderRepo(r), orderSequence(seq), productRepo(p) {}


	int createOrder(int userId, vector<int>& productIds) {
		Order order{ orderSequence++, userId, productIds };
		orderRepo.save(order);
		return order.orderId;
	}

	Order* getOrder(int orderId) {
		return orderRepo.findById(orderId);
	}

	//Add 1
	int getProductCount(int orderId) {
		Order* order = orderRepo.findById(orderId);
		if (!order) throw runtime_error("Order not found");
		return order->productIds.size();
	}

	void removeOrder(int orderId) {
		orderRepo.remove(orderId);
	}

	// Add 2: phep join thu cong
	/*void printOrderDetails(int orderId) {
		Order* order = orderRepo.findById(orderId);
		if (!order) {
			cout << "Order not found\n";
			return;
		}
		cout << "Order " << orderId << " for user " << order->userId << ":\n";
		for (int productId : order->productIds) {
			Product* p = productRepo.findById(productId);
			if (p)
				cout << "- " << p->name << " ($" << p->price << ")\n";
			else
				cout << "- Unknown product ID: " << productId << "\n";
		}
	}*/
};

//controller
class OrderController {
private:
	OrderService& service;
	OrderFormatter& formatter;
public:
	OrderController(OrderService& s, OrderFormatter& f) : service(s), formatter(f) {}

	void createOrder(int userId, vector<int>& productIds) {
		int id = service.createOrder(userId, productIds);
		cout << "Order created with Id: " << id << "\n";
	}

	void viewOrder(int orderId) {
		auto order = service.getOrder(orderId);
		if (order) {
			std::cout << "Order ID: " << order->orderId
				<< ", User ID: " << order->userId << "\nProducts: ";
			for (int pid : order->productIds)
				std::cout << pid << " ";
			std::cout << "\n";
		}
		else {
			std::cout << "Order not found.\n";
		}

	}

	// Add 1
	void showProductCount(int orderId) {
		try {
			int count = service.getProductCount(orderId);
			cout << "Order " << orderId << " has " << count << " product(s).\n";
		}
		catch (const std::exception& e) {
			cout << e.what() << "\n";
		}
	}

	void removeOrder(int orderId) {
		service.removeOrder(orderId);
		cout << "Order " << orderId << " deleted.\n";
	}

	//Add 2
	void viewOrderDetails(int orderId) {
		/*service.printOrderDetails(orderId);*/
		Order* order = service.getOrder(orderId);
		if (order)
			formatter.print(*order);
		else
			cout << "Order not found\n";
	}
};
