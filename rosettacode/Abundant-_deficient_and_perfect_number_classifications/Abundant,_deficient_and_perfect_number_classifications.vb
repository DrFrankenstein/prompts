Module NumberClassification
    Function SumOfDivisors(n As Integer) As Integer
        If n = 1 Then Return 0

        Dim limit = Math.Sqrt(n)
        If limit < 2 Then Return 1

        Return (Aggregate i In Enumerable.Range(2, CInt(limit) - 1)
               Where n Mod i = 0
               Select If(CDbl(i) = limit, i, i + n \ i)
               Into Sum()) + 1
    End Function

    Sub main(args() As String)
        Dim DeficientCount = 0, PerfectCount = 0, AbundantCount = 0

        For n = 1 To 20000
            Dim pn = SumOfDivisors(n)
            If pn < n Then
                DeficientCount += 1
            ElseIf pn = n Then
                PerfectCount += 1
            Else
                AbundantCount += 1
            End If
        Next n

        Console.Write(
            "Deficient integers: {0}" & vbNewLine &
            "Perfect integers: {1}" & vbNewLine &
            "Abundant integers: {2}",
            DeficientCount, PerfectCount, AbundantCount
        )
    End Sub
End Module
