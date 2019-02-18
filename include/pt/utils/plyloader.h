#ifndef PT_UTILS_PLYLOADER_H
#define PT_UTILS_PLYLOADER_H

#include <map>
#include <vector>
#include <memory>
#include <sstream>
#include <fstream>

namespace pt {

enum class TypeId {
    INT8, UINT8, INT16, UINT16, INT32, UINT32, FLOAT, DOUBLE, UNKNOWN
};

template<typename T> constexpr TypeId typeId()     { return TypeId::UNKNOWN; }
template<> constexpr TypeId typeId<int8_t>()       { return TypeId::INT8; }
template<> constexpr TypeId typeId<uint8_t>()      { return TypeId::UINT8; }
template<> constexpr TypeId typeId<int16_t>()      { return TypeId::INT16; }
template<> constexpr TypeId typeId<uint16_t>()     { return TypeId::UINT16; }
template<> constexpr TypeId typeId<int32_t>()      { return TypeId::INT32; }
template<> constexpr TypeId typeId<uint32_t>()     { return TypeId::UINT32; }
template<> constexpr TypeId typeId<float>()        { return TypeId::FLOAT; }
template<> constexpr TypeId typeId<double>()       { return TypeId::DOUBLE; }

template<typename T> std::string typeName()       { return "unknown"; }
template<> std::string typeName<int8_t>()         { return "char"; }
template<> std::string typeName<uint8_t>()        { return "uchar"; }
template<> std::string typeName<int16_t>()        { return "short";   }
template<> std::string typeName<uint16_t>()       { return "ushort";  }
template<> std::string typeName<int32_t>()        { return "int";     }
template<> std::string typeName<uint32_t>()       { return "uint";    }
template<> std::string typeName<float>()          { return "float";   }
template<> std::string typeName<double>()         { return "double";  }

class Property {
public:
    explicit Property(const std::string& name) noexcept : name(name)
    { }

    virtual ~Property() = default;
    virtual void parse(std::istringstream& lineStream) = 0;
    virtual void read(std::ifstream& stream) = 0;
    virtual void writeHeader(std::ofstream& stream) const = 0;
    virtual void writeText(std::ofstream& stream, size_t iElement) const = 0;
    virtual void writeBinary(std::ofstream& stream, size_t iElement) const = 0;
    virtual size_t size() const = 0;

protected:
    friend class Element;
    std::string name;
};

template <typename T>
class TypedProperty : public Property {
public:
    static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown property type.");

    explicit TypedProperty(const std::string& name) noexcept : Property(name)
    {}

    TypedProperty(const std::string& name, const std::vector<T>& data) : Property(name), data(data)
    { }

    void parse(std::istringstream& lineStream) override {
        T val;
        lineStream >> val;
        data.push_back(val);
    }

    void read(std::ifstream& stream) override {
        T val;
        stream.read((char*)&val, sizeof(T));
        data.push_back(val);
    }

    void writeHeader(std::ofstream& stream) const override {
        stream << "property " << typeName<T>() << " " << name << std::endl;
    }

    void writeText(std::ofstream& stream, size_t iElement) const override {
        stream.precision(std::numeric_limits<T>::max_digits10);
        stream << data[iElement] << " ";
    }

    void writeBinary(std::ofstream& stream, size_t iElement) const override {
        stream.write((char*)&data[iElement], sizeof(T));
    }

    size_t size() const override {
        return data.size();
    }

private:
    friend class Element;
    std::vector<T> data;
};

template <>
void TypedProperty<uint8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    stream << (int)data[iElement] << " ";
}

template <>
void TypedProperty<int8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    stream << (int)data[iElement] << " ";
}

template <>
void TypedProperty<uint8_t>::parse(std::istringstream &lineStream) {
    int val;
    lineStream >> val;
    data.push_back((uint8_t)val);
}

template <>
void TypedProperty<int8_t>::parse(std::istringstream &lineStream) {
    int val;
    lineStream >> val;
    data.push_back((int8_t)val);
}

template <typename T>
class TypedListProperty : public Property {
public:
    static_assert(typeId<T>() != TypeId::UNKNOWN, "PLY Loader: unknown list property type.");

    explicit TypedListProperty(const std::string& name, int listCountBytes = 1)
        : Property(name), listCountBytes(listCountBytes)
    { }

    TypedListProperty(const std::string& name, std::vector<std::vector<T>>& data)
        : Property(name), data(data)
    { }

    void writeHeader(std::ofstream& stream) const override {
        stream << "property list uchar " << typeName<T>() << " " << name << std::endl;
    }

    void parse(std::istringstream& lineStream) override {
        int count;
        lineStream >> count;
        T val;
        std::vector<T> vec;
        for (auto i = 0; i < count; ++i) {
            lineStream >> val;
            vec.push_back(val);
        }
        data.push_back(std::move(vec));
    }

    void read(std::ifstream& stream) override {
        int count = 0;
        stream.read((char*)&count, listCountBytes);
        T val;
        std::vector<T> vec;
        for (auto i = 0; i < count; ++i) {
            stream.read((char*)&val, sizeof(T));
            vec.push_back(val);
        }
        data.push_back(std::move(vec));
    }

    void writeText(std::ofstream& stream, size_t iElement) const override {
        auto& elemList = data[iElement];
        auto count = (uint8_t)elemList.size();
        if (count != elemList.size())
            throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
        stream << elemList.size();
        stream.precision(std::numeric_limits<T>::max_digits10);
        for (auto e : elemList) stream << " " << e;
        stream << " ";
    }

    void writeBinary(std::ofstream& stream, size_t iElement) const override {
        auto& elemList = data[iElement];
        auto count = (uint8_t)elemList.size();
        if (count != elemList.size())
            throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
        stream.write((char*)&count, sizeof(uint8_t));
        for (auto e : elemList) stream.write((char*)&e, sizeof(T));
    }

    size_t size() const override {
        return data.size();
    }

private:
    friend class Element;
    int listCountBytes;
    std::vector<std::vector<T>> data;
};

template <>
void TypedListProperty<uint8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    auto& elemList = data[iElement];
    auto count = (uint8_t)elemList.size();
    if (count != elemList.size())
        throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
    stream << elemList.size();
    for (auto e : elemList) stream << " " << (int)e;
    stream << " ";
}

template <>
void TypedListProperty<int8_t>::writeText(std::ofstream &stream, size_t iElement) const {
    auto& elemList = data[iElement];
    auto count = (uint8_t)elemList.size();
    if (count != elemList.size())
        throw std::runtime_error("PLY loader: list property has a element with more entries than fit in a uchar.");
    stream << elemList.size();
    for (auto e : elemList) stream << " " << (int)e;
    stream << " ";
}

template <>
void TypedListProperty<uint8_t>::parse(std::istringstream &lineStream) {
    int count;
    lineStream >> count;
    int val;
    std::vector<uint8_t> vec;
    for (auto i = 0; i < count; ++i) {
        lineStream >> val;
        vec.push_back((uint8_t)val);
    }
    data.push_back(std::move(vec));
}

template <>
void TypedListProperty<int8_t>::parse(std::istringstream &lineStream) {
    int count;
    lineStream >> count;
    int val;
    std::vector<int8_t> vec;
    for (auto i = 0; i < count; ++i) {
        lineStream >> val;
        vec.push_back((uint8_t)val);
    }
    data.push_back(std::move(vec));
}

std::unique_ptr<Property> createProperty(
        bool isList, const std::string& name,
        const std::string& typeStr, const std::string& listCountTypeStr) {

    auto listCountBytes = 0;

    if (isList) {
        if (listCountTypeStr == "uchar" || listCountTypeStr == "uint8" ||
            listCountTypeStr == "char" || listCountTypeStr == "int8")
            listCountBytes = 1;
        else if (listCountTypeStr == "ushort" || listCountTypeStr == "uint16" ||
                 listCountTypeStr == "short" || listCountTypeStr == "int16")
            listCountBytes = 2;
        else if (listCountTypeStr == "uint" || listCountTypeStr == "uint32" ||
                 listCountTypeStr == "int" || listCountTypeStr == "int32")
            listCountBytes = 4;
        else
            throw std::runtime_error("PLY loader: unrecognized list count type: " + listCountTypeStr + ".");
    }

    if (typeStr == "uchar" || typeStr == "uint8") {
        return isList ?
            std::unique_ptr<Property>(new TypedListProperty<uint8_t>(name, listCountBytes)) :
            std::unique_ptr<Property>(new TypedProperty<uint8_t>(name));
    } else if (typeStr == "ushort" || typeStr == "uint16") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<uint16_t>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<uint16_t>(name));
    } else if (typeStr == "uint" || typeStr == "uint32") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<uint32_t>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<uint32_t>(name));
    }

    if (typeStr == "char" || typeStr == "int8") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<int8_t>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<int8_t>(name));
    } else if (typeStr == "short" || typeStr == "int16") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<int16_t>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<int16_t>(name));
    } else if (typeStr == "int" || typeStr == "int32") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<int32_t>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<int32_t>(name));
    }

    if (typeStr == "float" || typeStr == "float32") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<float>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<float>(name));
    } else if (typeStr == "double" || typeStr == "float64") {
        return isList ?
           std::unique_ptr<Property>(new TypedListProperty<double>(name, listCountBytes)) :
           std::unique_ptr<Property>(new TypedProperty<double>(name));
    }

    throw std::runtime_error("PLY loader:  unrecognized data type: " + typeStr + ".");
}

class Element {
public:
    Element(const std::string& name, size_t count) : name(name), count(count)
    { }

    void addProperty(std::unique_ptr<Property>&& property) {
        if (hasProperty(property->name))
            throw std::runtime_error("PLY loader: element already has property with name: " + name + ".");
        properties.emplace(name, std::move(property));
    }

    template <typename T>
    void addProperty(const std::string& name, std::vector<T>& data) {
        if (data.size() != count)
            throw std::runtime_error("PLY loader: new property " + name + " has size which does not match element");
        if (hasProperty(name))
            throw std::runtime_error("PLY loader: element already has property with name: " + name + ".");
        properties.emplace(name, std::unique_ptr<Property>(new TypedProperty<T>(name, data)));
    }

    template <typename T>
    void addListProperty(const std::string& name, std::vector<std::vector<T>>& data) {
        if (data.size() != count)
            throw std::runtime_error("PLY loader: new property " + name + " has size which does not match element");
        if (hasProperty(name))
            throw std::runtime_error("PLY loader: element already has property with name: " + name + ".");
        properties.emplace(name, std::unique_ptr<Property>(new TypedListProperty<T>(data)));
    }

    bool hasProperty(const std::string& name) const {
        return properties.find(name) != properties.end();
    }

    template <typename T>
    std::vector<T> getProperty(const std::string& name) {
        if (!hasProperty(name))
            throw std::out_of_range("PLY loader: element hove no property with name: " + name + ".");
        return dynamic_cast<TypedListProperty<T>*>(properties[name].get())->data;
    }

    template <typename T>
    std::vector<std::vector<T>> getListProperty(const std::string& name) {
        if (!hasProperty(name))
            throw std::out_of_range("PLY loader: element hove no property with name: " + name + ".");
        return dynamic_cast<TypedListProperty<T>*>(properties[name].get())->data;
    }

    void writeHeader(std::ofstream& stream) const {
        stream << "element " << name << " " << count << std::endl;
        for (auto& entry : properties)
            entry.second->writeHeader(stream);
    }

    void writeText(std::ofstream& stream) const {
        for (size_t i = 0; i < count; ++i) {
            for (auto& entry : properties)
                entry.second->writeText(stream, i);
            stream << std::endl;
        }
    }

    void writeBinary(std::ofstream& stream) const {
        for (size_t i = 0; i < count; ++i)
            for (auto& entry : properties)
                entry.second->writeBinary(stream, i);
    }

private:
    friend class PLYData;
    std::string name;
    size_t count;
    std::map<std::string, std::unique_ptr<Property>> properties;
};

enum class DataFormat { Text, Binary };

class PLYData {
public:
    PLYData() noexcept = default;

    explicit PLYData(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.fail()) throw std::runtime_error("PLY loader: could not open file " + filename + ".");
        parseHeader(file);
        if (format == DataFormat::Text) parseTextBody(file);
        else parseBinaryBody(file);
    }

    void parseHeader(std::istream& stream) {
        std::string line;
        while (std::getline(stream, line)) {
            std::istringstream lineStream(line);
            std::string token;
            lineStream >> token;
            if (token == "ply" || token == "PLY" || token == "comment") continue;
            else if (token == "format") parseFormatHeader(lineStream);
            else if (token == "element") parseElementHeader(lineStream);
            else if (token == "property") parsePropertyHeader(lineStream);
            else if (token == "end_header") break;
        }
    }

    void parseFormatHeader(std::istringstream& stream) {
        std::string type, version;
        stream >> type >> version;
        if (version != "1.0") throw std::runtime_error("PLY loader: only version 1.0 is supported.");
        if (type == "ascii") format = DataFormat::Text;
        else if (type == "binary_little_endian") format = DataFormat::Binary;
        else if (type == "binary_big_endian") throw std::runtime_error("PLY loader: big endian binary format not supported.");
        else throw std::runtime_error("PLY loader: unknown file format.");
    }

    void parseElementHeader(std::istringstream& stream) {
        std::string name;
        size_t count;
        stream >> name >> count;
        addElement(name, count);
    }

    void parsePropertyHeader(std::istringstream& stream) {
        if (!lastElement) throw std::runtime_error("PLY loader: encounter property header before any element header.");
        std::string firstToken;
        stream >> firstToken;
        if (firstToken == "list") {
            std::string countType, type, name;
            stream >> countType >> type >> name;
            lastElement->addProperty(createProperty(true, name, type, countType));
        } else {
            std::string type, name;
            type = firstToken;
            stream >> name;
            lastElement->addProperty(createProperty(false, name, type, ""));
        }
    }

    void parseBinaryBody(std::ifstream& stream) {
        for (auto& element : elements)
            for (auto i = 0; i < element.second.count; ++i)
                for (auto& property : element.second.properties)
                    property.second->read(stream);
    }

    void parseTextBody(std::ifstream& stream) {
        for (auto& element : elements)
            for (auto i = 0; i < element.second.count; ++i) {
                std::string line;
                std::getline(stream, line);
                std::istringstream lineStream(line);
                for (auto& property : element.second.properties)
                    property.second->parse(lineStream);
            }
    }

    void write(const std::string& filename, DataFormat format = DataFormat::Text) const {
        std::ofstream file(filename, std::ios::out | std::ios::binary);
        if (file.fail()) throw std::runtime_error("PLY loader: count not open file " + filename + ".");
        writeHeader(file, format);
        for (auto& entry : elements) {
            if (format == DataFormat::Text) entry.second.writeText(file);
            else entry.second.writeBinary(file);
        }
    }

    void writeHeader(std::ofstream& stream, DataFormat format) const {
        stream << "ply" << std::endl;
        stream << "format ";
        if (format == DataFormat::Text) stream << "ascii ";
        else stream << "binary_little_endian ";
        stream << "1.0" << std::endl;
        for (auto& entry : elements) entry.second.writeHeader(stream);
        stream << "end_header" << std::endl;
    }

    bool hasElement(const std::string& name) const {
        return elements.find(name) != elements.end();
    }

    Element& getElement(const std::string& name) {
        if (!hasElement(name))
            throw std::runtime_error("PLY loader: count not find element with name: " + name + ".");
        return elements.at(name);
    }

    void addElement(const std::string& name, size_t count) {
        if (hasElement(name))
            throw std::runtime_error("PLY loader: element " + name + "already exist.");
        elements.emplace(name, Element(name, count));
        lastElement = &elements.at(name);
    }

private:
    Element* lastElement = nullptr;
    DataFormat format = DataFormat::Text;
    std::map<std::string, Element> elements;
};

}

#endif
