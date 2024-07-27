module;

#include <choir/macros.hh>
#include <llvm/ADT/SmallVector.h>
#include <memory>

export module choir.frontend.laye:syntax;
import choir;
import choir.frontend;

export namespace choir::laye {

class SyntaxModule {
    CHOIR_IMMOVABLE(SyntaxModule);

public:
    using Ptr = std::unique_ptr<SyntaxModule>;

private:
    explicit SyntaxModule();

public:
    static auto Create(String name) -> Ptr;
};

class SyntaxNode {
    CHOIR_IMMOVABLE(SyntaxNode);

public:
    enum struct Kind {
        Invalid,
    };

private:
    Kind _kind;

public:
    SyntaxNode(Kind kind) : _kind(kind) {}
};

}; // namespace choir::laye