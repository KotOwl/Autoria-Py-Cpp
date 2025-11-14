// FILE: backend/src/repositories/BrandRepository.h
#pragma once
#include "../models/Brand.h"
#include "../database/Database.h"
#include <memory>
#include <vector>

class BrandRepository {
private:
    std::shared_ptr<Database> db_;

public:
    BrandRepository(std::shared_ptr<Database> db);
    
    std::vector<std::unique_ptr<Brand>> getAll();
    std::unique_ptr<Brand> findById(int id);
    bool create(const std::string& name);
    bool update(int id, const std::string& name);
    bool deleteBrand(int id);
};

class ModelRepository {
private:
    std::shared_ptr<Database> db_;

public:
    ModelRepository(std::shared_ptr<Database> db);
    
    std::vector<std::unique_ptr<Model>> getByBrandId(int brandId);
    std::unique_ptr<Model> findById(int id);
    bool create(int brandId, const std::string& name);
    bool update(int id, const std::string& name);
    bool deleteModel(int id);
};



