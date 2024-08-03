using System.Diagnostics;
using System.Numerics;
using System.Text;
using ClangSharp;

namespace Choir.Front.Laye.Syntax;

public sealed class Lexer(SourceFile sourceFile)
{
    private static readonly Dictionary<string, SyntaxKind> _keywordTokensKinds = new()
    {
        {"var", SyntaxKind.TokenVar},
        {"void", SyntaxKind.TokenVoid},
        {"noreturn", SyntaxKind.TokenNoReturn},
        {"bool", SyntaxKind.TokenBool},
        {"int", SyntaxKind.TokenInt},
        {"true", SyntaxKind.TokenTrue},
        {"false", SyntaxKind.TokenFalse},
        {"nil", SyntaxKind.TokenNil},
        {"if", SyntaxKind.TokenIf},
        {"else", SyntaxKind.TokenElse},
        {"for", SyntaxKind.TokenFor},
        {"while", SyntaxKind.TokenWhile},
        {"do", SyntaxKind.TokenDo},
        {"switch", SyntaxKind.TokenSwitch},
        {"case", SyntaxKind.TokenCase},
        {"default", SyntaxKind.TokenDefault},
        {"return", SyntaxKind.TokenReturn},
        {"break", SyntaxKind.TokenBreak},
        {"continue", SyntaxKind.TokenContinue},
        {"fallthrough", SyntaxKind.TokenFallthrough},
        {"yield", SyntaxKind.TokenYield},
        {"unreachable", SyntaxKind.TokenUnreachable},
        {"defer", SyntaxKind.TokenDefer},
        {"discard", SyntaxKind.TokenDiscard},
        {"goto", SyntaxKind.TokenGoto},
        {"xyzzy", SyntaxKind.TokenXyzzy},
        {"assert", SyntaxKind.TokenAssert},
        {"try", SyntaxKind.TokenTry},
        {"catch", SyntaxKind.TokenCatch},
        {"struct", SyntaxKind.TokenStruct},
        {"variant", SyntaxKind.TokenVariant},
        {"enum", SyntaxKind.TokenEnum},
        {"alias", SyntaxKind.TokenAlias},
        {"template", SyntaxKind.TokenTemplate},
        {"test", SyntaxKind.TokenTest},
        {"import", SyntaxKind.TokenImport},
        {"export", SyntaxKind.TokenExport},
        {"operator", SyntaxKind.TokenOperator},
        {"mut", SyntaxKind.TokenMut},
        {"new", SyntaxKind.TokenNew},
        {"delete", SyntaxKind.TokenDelete},
        {"cast", SyntaxKind.TokenCast},
        {"is", SyntaxKind.TokenIs},
        {"sizeof", SyntaxKind.TokenSizeof},
        {"alignof", SyntaxKind.TokenAlignof},
        {"offsetof", SyntaxKind.TokenOffsetof},
        {"not", SyntaxKind.TokenNot},
        {"and", SyntaxKind.TokenAnd},
        {"or", SyntaxKind.TokenOr},
        {"xor", SyntaxKind.TokenXor},
        {"varargs", SyntaxKind.TokenVarargs},
        {"const", SyntaxKind.TokenConst},
        {"foreign", SyntaxKind.TokenForeign},
        {"inline", SyntaxKind.TokenInline},
        {"callconv", SyntaxKind.TokenCallconv},
        {"pure", SyntaxKind.TokenPure},
        {"discardable", SyntaxKind.TokenDiscardable},
    };

    public SourceFile SourceFile { get; } = sourceFile;
    public ChoirContext Context { get; } = sourceFile.Context;

    private readonly int _fileId = sourceFile.FileId;
    private readonly string _text = sourceFile.Text;
    private readonly int _textLength = sourceFile.Text.Length;

    private struct TokenInfo
    {
        public SyntaxKind Kind;
        public int Position;
        public int Length;
        public string TextValue = "";
        public BigInteger IntegerValue = BigInteger.Zero;

        public TokenInfo()
        {
        }
    }

    private enum ScanNumberFlags
    {
        NotNumber,
        VanillaInteger,
        RadixInteger,
        VanillaFloat,
        RadixFloat,
    }

    private class LexerResetPoint(Lexer lexer) : IDisposable
    {
        private readonly Lexer _lexer = lexer;
        private readonly int _position = lexer._position;

        public void Dispose()
        {
            _lexer._position = _position;
        }
    }

    private int _position;
    private readonly StringBuilder _stringBuilder = new();

    private bool IsAtEnd => _position >= _textLength;
    private char CurrentCharacter => Peek(0);
    private Location CurrentLocation => new(_position, 1, _fileId);

    private char Peek(int ahead)
    {
        Debug.Assert(ahead >= 0);

        int peekPosition = _position + ahead;
        if (peekPosition >= _textLength)
            return '\0';

        return _text[peekPosition];
    }

    private void Advance()
    {
        _position++;
    }

    private bool TryAdvance(char c)
    {
        if (CurrentCharacter != c)
            return false;

        Advance();
        return true;
    }

    public SyntaxToken ReadToken()
    {
        TokenInfo tokenInfo = default;
        ReadTokenInfo(ref tokenInfo);
        return new SyntaxToken(tokenInfo.Kind, new Location(tokenInfo.Position, tokenInfo.Length, _fileId))
        {
            TextValue = tokenInfo.TextValue,
            IntegerValue = tokenInfo.IntegerValue,
        };
    }

    private void ReadTokenInfo(ref TokenInfo tokenInfo)
    {
        SkipTrivia();

        tokenInfo.Kind = SyntaxKind.Invalid;
        tokenInfo.Position = _position;

        switch (CurrentCharacter)
        {
            case '_':
            case (>= 'a' and <= 'z') or (>= 'A' and <= 'Z'):
            {
                ReadIdentifier(ref tokenInfo);
                if (_keywordTokensKinds.TryGetValue(tokenInfo.TextValue, out var keywordKind))
                    tokenInfo.Kind = keywordKind;
            } break;

            case >= '0' and <= '9':
            {
                switch (ScanNumber())
                {
                    default: throw new UnreachableException();
                    case ScanNumberFlags.NotNumber: ReadIdentifier(ref tokenInfo); break;
                    case ScanNumberFlags.VanillaInteger: ReadInteger(ref tokenInfo); break;
                    case ScanNumberFlags.RadixInteger: ReadIntegerWithRadix(ref tokenInfo); break;
                    case ScanNumberFlags.VanillaFloat: ReadFloat(ref tokenInfo); break;
                    case ScanNumberFlags.RadixFloat: ReadFloatWithRadix(ref tokenInfo); break;
                }
            } break;

            default:
            {
            } break;
        }

        tokenInfo.Length = _position - tokenInfo.Position;
        SkipTrivia();
    }

    private void SkipTrivia()
    {
        while (!IsAtEnd)
        {
            while (SyntaxFacts.IsWhiteSpaceCharacter(CurrentCharacter))
                Advance();

            switch (CurrentCharacter)
            {
                default: return;

                case '/' when Peek(1) == '/':
                    {
                        Advance();
                        Advance();
                        while (!IsAtEnd && CurrentCharacter != '\n')
                            Advance();
                    }
                    break;

                case '/' when Peek(1) == '*':
                    {
                        var location = CurrentLocation;

                        Advance();
                        Advance();

                        int nesting = 1;
                        while (!IsAtEnd && nesting > 0)
                        {
                            if (CurrentCharacter == '/' && Peek(1) == '*')
                            {
                                Advance();
                                Advance();
                                nesting++;
                            }
                            else if (CurrentCharacter == '*' && Peek(1) == '/')
                            {
                                Advance();
                                Advance();
                                nesting--;
                            }
                            else Advance();
                        }

                        if (nesting > 0)
                            Context.Diag.Error(location, "comment unclosed at end of file");
                    }
                    break;
            }
        }
    }

    private void ReadIdentifier(ref TokenInfo tokenInfo)
    {
        Debug.Assert(SyntaxFacts.IsIdentifierStartCharacter(CurrentCharacter));

        tokenInfo.Kind = SyntaxKind.TokenIdentifier;
        while (!IsAtEnd)
        {
            if (CurrentCharacter is '_' or (>= 'a' and <= 'z') or (>= 'A' and <= 'Z') or (>= '0' and <= '9'))
                Advance();
            else
            {
                if (SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter))
                    Advance();
                else break;
            }
        }

        tokenInfo.TextValue = _text.Substring(tokenInfo.Position, _position - tokenInfo.Position);
    }

    private void ReadInteger(ref TokenInfo tokenInfo)
    {
        Debug.Assert(CurrentCharacter is >= '0' and <= '9');

        // when reading *just* an integer, we should be able to assert that
        // there are no identifier characters within or at its end.
        // the scanning process would trivially discard this case.
        
        tokenInfo.Kind = SyntaxKind.TokenLiteralInteger;

        _stringBuilder.Clear();
        while (!IsAtEnd && CurrentCharacter is (>= '0' and <= '9') or '_')
        {
            if (CurrentCharacter != '_')
                _stringBuilder.Append(CurrentCharacter);
            Advance();
        }

        Debug.Assert(CurrentCharacter is not ((>= '0' and <= '9') or (>= 'a' and <= 'z') or (>= 'A' and <= 'Z') or '_') && !SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter));
        
        tokenInfo.IntegerValue = BigInteger.Parse(_stringBuilder.ToString());
    }

    private void ReadIntegerWithRadix(ref TokenInfo tokenInfo)
    {
        Debug.Assert(CurrentCharacter is >= '0' and <= '9');

        var startLocation = CurrentLocation;
        tokenInfo.Kind = SyntaxKind.TokenLiteralInteger;

        int radix;
        if (Peek(1) == '#')
        {
            radix = CurrentCharacter - '0';
            if (radix < 2)
            {
                Context.Diag.Error(startLocation, "integer base must be in the range [2, 36]");
                radix = 2;
            }

            Advance();
        }
        else if (Peek(1) is >= '0' and <= '9' && Peek(2) == '#')
        {
            radix = 10 * (CurrentCharacter - '0') + (Peek(1) - '0');
            if (radix > 36)
            {
                Context.Diag.Error(startLocation, "integer base must be in the range [2, 36]");
                radix = 36;
            }

            Advance();
            Advance();
        }
        else
        {
            Context.Diag.Error(startLocation, "integer base must be in the range [2, 36]");
            radix = 36;
            
            bool reportedIllegalUnderscore = false;
            while (CurrentCharacter != '#')
            {
                if (CurrentCharacter == '_' && !reportedIllegalUnderscore)
                {
                    Context.Diag.Error(CurrentLocation, "integer base must not contain '_' separators");
                    reportedIllegalUnderscore = true;
                }

                Advance();
            }
        }

        Debug.Assert(!IsAtEnd && CurrentCharacter == '#');
        Debug.Assert(radix is >= 2 and <= 36);

        _stringBuilder.Clear();
        
        bool wasLastCharacterUnderscore = false;
        bool parseResult = true;

        while (!IsAtEnd && CurrentCharacter is (>= '0' and <= '9') or (>= 'a' and <= 'z') or (>= 'A' and <= 'Z') or '_' || SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter))
        {
            if (CurrentCharacter == '_')
            {
                while (CurrentCharacter == '_') Advance();
                wasLastCharacterUnderscore = true;
                continue;
            }
            else if (SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter))
            {
                Context.Diag.Error(CurrentLocation, "invalid digit in integer literal");
                while (SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter)) Advance();
                return;
            }

            if (!SyntaxFacts.IsNumericLiteralDigitInRadix(CurrentCharacter, radix))
            {
                Context.Diag.Error(CurrentLocation, $"invalid digit in base {radix} integer literal");
                parseResult = false;
                _stringBuilder.Clear();
            }
            
            wasLastCharacterUnderscore = false;
            if (parseResult) _stringBuilder.Append(CurrentCharacter);
            Advance();
        }

        if (wasLastCharacterUnderscore)
            Context.Diag.Error(startLocation, "integer literal cannot end with an '_' separator");

        if (parseResult)
        {
            Debug.Assert(_stringBuilder.Length > 0);
            tokenInfo.IntegerValue = ParseBigInteger(_stringBuilder.ToString(), radix);
        }
        else
        {
            Debug.Assert(_stringBuilder.Length == 0);
        }

        static BigInteger ParseBigInteger(string value, int radix)
        {
            if (radix == 16) return BigInteger.Parse(value, System.Globalization.NumberStyles.HexNumber);
            var result = BigInteger.Zero;
            for (int i = 0; i < value.Length; i++)
                result = result * radix + SyntaxFacts.NumericLiteralDigitValueInRadix(value[i], radix);
            return result;
        }
    }

    private void ReadFloat(ref TokenInfo tokenInfo)
    {
        Context.Diag.Fatal("float literal tokens are currently not supported :(");
        throw new UnreachableException();
    }

    private void ReadFloatWithRadix(ref TokenInfo tokenInfo)
    {
        Context.Diag.Fatal("float literal tokens are currently not supported :(");
        throw new UnreachableException();
    }

    private ScanNumberFlags ScanNumber()
    {
        Debug.Assert(SyntaxFacts.IsNumericLiteralDigit(CurrentCharacter));

        using var resetPoint = new LexerResetPoint(this);
        while (!IsAtEnd)
        {
            switch (CurrentCharacter)
            {
                case >= '0' and <= '9': Advance(); break;
                case '_' when IsSequenceOfUnderscoresFollowedByDigit():
                {
                    while (CurrentCharacter == '_') Advance();
                    Advance(); // the digit
                } break;

                // an underscore with no following digit, or any letters, means this is
                // definitively not going to be a number.
                case '_':
                case (>= 'a' and <= 'z') or (>= 'A' and <= 'Z'):
                    return ScanNumberFlags.NotNumber;

                case '.': return ScanNumberFlags.VanillaFloat;
                case '#': return ScanNumberWithRadix();
                default: return SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter) ? ScanNumberFlags.NotNumber : ScanNumberFlags.VanillaInteger;
            }
        }

        return ScanNumberFlags.VanillaInteger;

        bool IsSequenceOfUnderscoresFollowedByDigit()
        {
            for (int i = 0; i < 10; i++)
            {
                if (Peek(i) != '_') return false;
                if (Peek(i + 1) is >= '0' and <= '9') return true;
            }

            return false;
        }

        ScanNumberFlags ScanNumberWithRadix()
        {
            Debug.Assert(CurrentCharacter == '#');
            Advance();

            while (!IsAtEnd)
            {
                // at this point we're beyond the point of no return for identifier fallbacks
                if (CurrentCharacter is (>= '0' and <= '9') or (>= 'a' and <= 'z') or (>= 'A' and <= 'Z') || SyntaxFacts.IsIdentifierPartCharacter(CurrentCharacter))
                    Advance();
                else if (CurrentCharacter == '.')
                    return ScanNumberFlags.RadixFloat;
                else break;
            }

            return ScanNumberFlags.RadixInteger;
        }
    }
}