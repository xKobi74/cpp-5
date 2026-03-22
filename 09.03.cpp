/*
g++ -std=c++23 -Wall -Wextra -Wpedantic 09.03.cpp -o 09.03.out
./09.03.out
*/

#include <cstddef>
#include <cassert>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

namespace example_05_01
{
    struct Entity
    {
        int m_x = 0;
        int m_y = 0;
    };

///////////////////////////////////////////////////////////////////////

    class Builder
    {
    public:

        virtual ~Builder() = default;

//      ----------------------------

        std::unique_ptr<Entity> make_entity()
        {
            m_entity = std::make_unique<Entity>();

            set_x();
            set_y();

            return std::move(m_entity);
        }

//      ----------------------------

        virtual void set_x() const = 0;
        virtual void set_y() const = 0;

    protected:

        mutable std::unique_ptr<Entity> m_entity{};
    };

///////////////////////////////////////////////////////////////////////

    class Builder_Client : public Builder
    {
    public:

        void set_x() const override
        {
            m_entity->m_x = 1;
        }

//      ----------------------------

        void set_y() const override
        {
            m_entity->m_y = 1;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Builder_Server : public Builder
    {
    public:

        void set_x() const override
        {
            m_entity->m_x = 2;
        }

//      ----------------------------

        void set_y() const override
        {
            m_entity->m_y = 2;
        }
    };

///////////////////////////////////////////////////////////////////////

    static void run()
    {
        std::unique_ptr<Builder> builder = std::make_unique<Builder_Client>();
        std::unique_ptr<Entity> entity = builder->make_entity();

        assert(entity);
        assert(entity->m_x == 1);
        assert(entity->m_y == 1);

        builder = std::make_unique<Builder_Server>();
        entity = builder->make_entity();

        assert(entity);
        assert(entity->m_x == 2);
        assert(entity->m_y == 2);

        std::cout << "05.01 ok\n";
    }
}

///////////////////////////////////////////////////////////////////////

namespace example_05_03
{
    class Entity
    {
    public:

        virtual ~Entity() = default;

//      ----------------------------

        virtual int type() const = 0;
    };

///////////////////////////////////////////////////////////////////////

    class Client : public Entity
    {
    public:

        int type() const override
        {
            return 1;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Server : public Entity
    {
    public:

        int type() const override
        {
            return 2;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Factory
    {
    public:

        virtual ~Factory() = default;

//      ----------------------------

        virtual std::unique_ptr<Entity> make_entity() const = 0;
    };

///////////////////////////////////////////////////////////////////////

    class Factory_Client : public Factory
    {
    public:

        std::unique_ptr<Entity> make_entity() const override
        {
            return std::make_unique<Client>();
        }
    };

///////////////////////////////////////////////////////////////////////

    class Factory_Server : public Factory
    {
    public:

        std::unique_ptr<Entity> make_entity() const override
        {
            return std::make_unique<Server>();
        }
    };

///////////////////////////////////////////////////////////////////////

    static void run()
    {
        std::unique_ptr<Factory> factory = std::make_unique<Factory_Client>();
        std::unique_ptr<Entity> entity = factory->make_entity();

        assert(entity);
        assert(entity->type() == 1);

        factory = std::make_unique<Factory_Server>();
        entity = factory->make_entity();

        assert(entity);
        assert(entity->type() == 2);

        std::cout << "05.03 ok\n";
    }
}

///////////////////////////////////////////////////////////////////////

namespace example_05_04
{
    class Entity
    {
    public:

        virtual ~Entity() = default;

//      ----------------------------

        virtual std::unique_ptr<Entity> copy() const = 0;

//      ----------------------------

        virtual int type() const = 0;
    };

///////////////////////////////////////////////////////////////////////

    class Client : public Entity
    {
    public:

        std::unique_ptr<Entity> copy() const override
        {
            return std::make_unique<Client>(*this);
        }

//      ----------------------------

        int type() const override
        {
            return 1;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Server : public Entity
    {
    public:

        std::unique_ptr<Entity> copy() const override
        {
            return std::make_unique<Server>(*this);
        }

//      ----------------------------

        int type() const override
        {
            return 2;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Prototype
    {
    public:

        Prototype()
        {
            m_entities.push_back(std::make_unique<Client>());
            m_entities.push_back(std::make_unique<Server>());
        }

//      ----------------------------

        std::unique_ptr<Entity> make_client() const
        {
            return m_entities.at(0)->copy();
        }

//      ----------------------------

        std::unique_ptr<Entity> make_server() const
        {
            return m_entities.at(1)->copy();
        }

    private:

        std::vector<std::unique_ptr<Entity>> m_entities{};
    };

///////////////////////////////////////////////////////////////////////

    static void run()
    {
        Prototype prototype;

        std::unique_ptr<Entity> client = prototype.make_client();
        std::unique_ptr<Entity> server = prototype.make_server();

        assert(client);
        assert(server);
        assert(client->type() == 1);
        assert(server->type() == 2);
        assert(client.get() != server.get());

        std::cout << "05.04 ok\n";
    }
}

///////////////////////////////////////////////////////////////////////

namespace example_05_09
{
    class Entity
    {
    public:

        virtual ~Entity() = default;

//      ----------------------------

        virtual int test() const = 0;
    };

///////////////////////////////////////////////////////////////////////

    class Client : public Entity
    {
    public:

        int test() const override
        {
            return 1;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Server : public Entity
    {
    public:

        int test() const override
        {
            return 2;
        }
    };

///////////////////////////////////////////////////////////////////////

    class Composite : public Entity
    {
    public:

        void add(std::unique_ptr<Entity> entity)
        {
            m_entities.push_back(std::move(entity));
        }

//      ----------------------------

        int test() const override
        {
            int result = 0;

            for (const std::unique_ptr<Entity>& entity : m_entities)
            {
                if (entity)
                {
                    result += entity->test();
                }
            }

            return result;
        }

    private:

        std::vector<std::unique_ptr<Entity>> m_entities{};
    };

///////////////////////////////////////////////////////////////////////

    static std::unique_ptr<Entity> make_composite(
        const std::size_t client_count,
        const std::size_t server_count)
    {
        std::unique_ptr<Composite> composite = std::make_unique<Composite>();

        for (std::size_t index = 0; index < client_count; ++index)
        {
            composite->add(std::make_unique<Client>());
        }

        for (std::size_t index = 0; index < server_count; ++index)
        {
            composite->add(std::make_unique<Server>());
        }

        return composite;
    }

///////////////////////////////////////////////////////////////////////

    static void run()
    {
        std::unique_ptr<Composite> composite = std::make_unique<Composite>();

        constexpr std::size_t group_count = 5;
        constexpr std::size_t client_count = 1;
        constexpr std::size_t server_count = 1;
        constexpr int expected_value = 15;

        for (std::size_t index = 0; index < group_count; ++index)
        {
            composite->add(make_composite(client_count, server_count));
        }

        std::unique_ptr<Entity> entity = std::move(composite);

        assert(entity);
        assert(entity->test() == expected_value);

        std::cout << "05.09 ok\n";
    }
}

///////////////////////////////////////////////////////////////////////

namespace example_05_13
{
    class Observer
    {
    public:

        virtual ~Observer() = default;

//      ----------------------------

        virtual void test(const int value) const = 0;
    };

///////////////////////////////////////////////////////////////////////

    class Entity
    {
    public:

        void add(const std::shared_ptr<Observer>& observer)
        {
            m_observers.push_back(observer);
        }

//      ----------------------------

        void set(const int value)
        {
            m_value = value;
            notify_all();
        }

//      ----------------------------

        std::size_t observer_count() const
        {
            return m_observers.size();
        }

    private:

        void notify_all() const
        {
            for (const std::shared_ptr<Observer>& observer : m_observers)
            {
                if (observer)
                {
                    observer->test(m_value);
                }
            }
        }

    private:

        int m_value = 0;
        std::vector<std::shared_ptr<Observer>> m_observers{};
    };

///////////////////////////////////////////////////////////////////////

    class Client : public Observer
    {
    public:

        void test(const int value) const override
        {
            std::cout << "Client::test : x = " << value << '\n';
        }
    };

///////////////////////////////////////////////////////////////////////

    class Server : public Observer
    {
    public:

        void test(const int value) const override
        {
            std::cout << "Server::test : x = " << value << '\n';
        }
    };

///////////////////////////////////////////////////////////////////////

    static void run()
    {
        Entity entity;

        std::shared_ptr<Observer> client = std::make_shared<Client>();
        std::shared_ptr<Observer> server = std::make_shared<Server>();

        assert(client.use_count() == 1);
        assert(server.use_count() == 1);

        entity.add(client);
        entity.add(server);

        assert(entity.observer_count() == 2);
        assert(client.use_count() == 2);
        assert(server.use_count() == 2);

        for (int value = 1; value <= 2; ++value)
        {
            entity.set(value);
        }

        std::cout << "05.13 ok\n";
    }
}

///////////////////////////////////////////////////////////////////////

int main()
{
    example_05_01::run();
    example_05_03::run();
    example_05_04::run();
    example_05_09::run();
    example_05_13::run();

    std::cout << "tests passed\n";
}

///////////////////////////////////////////////////////////////////////