; ModuleID = 'bool-op-select.pp.bc'
source_filename = "bool-op-select.cpp"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-apple-macosx10.13.0"

; CHECK-LABEL: Bundle
; CHECK: target-endianness = little-endian
; CHECK: target-pointer-size = 64 bits
; CHECK: target-triple = x86_64-apple-macosx10.13.0

@a = global i32 0, align 4, !dbg !0
; CHECK: define si32* @a, align 4, init {
; CHECK: #1 !entry !exit {
; CHECK:   store @a, 0, align 1
; CHECK: }
; CHECK: }

; Function Attrs: noinline norecurse nounwind ssp uwtable
define i32 @main() #0 !dbg !12 {
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = load i32, i32* @a, align 4, !dbg !15
  %3 = icmp sgt i32 %2, 0, !dbg !16
  %4 = zext i1 %3 to i64, !dbg !15
  br i1 %3, label %5, label %6, !dbg !15

; <label>:5:                                      ; preds = %0
  br label %7, !dbg !15

; <label>:6:                                      ; preds = %0
  br label %7, !dbg !15

; <label>:7:                                      ; preds = %6, %5
  %8 = phi i32 [ 123, %5 ], [ 321, %6 ], !dbg !15
  ret i32 %8, !dbg !17
}
; CHECK: define si32 @main() {
; CHECK: #1 !entry successors={#2, #3} {
; CHECK:   si32* $1 = allocate si32, 1, align 4
; CHECK:   store $1, 0, align 4
; CHECK:   si32 %2 = load @a, align 4
; CHECK: }
; CHECK: #2 predecessors={#1} successors={#4} {
; CHECK:   %2 sigt 0
; CHECK:   ui1 %3 = 1
; CHECK: }
; CHECK: #3 predecessors={#1} successors={#4} {
; CHECK:   %2 sile 0
; CHECK:   ui1 %3 = 0
; CHECK: }
; CHECK: #4 predecessors={#2, #3} successors={#5, #6} {
; CHECK:   ui64 %4 = zext %3
; CHECK: }
; CHECK: #5 predecessors={#4} successors={#7} {
; CHECK:   %3 uieq 1
; CHECK:   si32 %5 = 123
; CHECK: }
; CHECK: #6 predecessors={#4} successors={#7} {
; CHECK:   %3 uieq 0
; CHECK:   si32 %5 = 321
; CHECK: }
; CHECK: #7 !exit predecessors={#5, #6} {
; CHECK:   return %5
; CHECK: }
; CHECK: }

attributes #0 = { noinline norecurse nounwind ssp uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "min-legal-vector-width"="0" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="penryn" "target-features"="+cx16,+fxsr,+mmx,+sahf,+sse,+sse2,+sse3,+sse4.1,+ssse3,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.dbg.cu = !{!2}
!llvm.module.flags = !{!7, !8, !9, !10}
!llvm.ident = !{!11}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(name: "a", scope: !2, file: !3, line: 8, type: !6, isLocal: false, isDefinition: true)
!2 = distinct !DICompileUnit(language: DW_LANG_C_plus_plus, file: !3, producer: "clang version 8.0.0 (tags/RELEASE_800/final)", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, enums: !4, globals: !5, nameTableKind: GNU)
!3 = !DIFile(filename: "bool-op-select.cpp", directory: "/Users/marthaud/ikos/ikos-git/frontend/llvm/test/regression/import/no_optimization")
!4 = !{}
!5 = !{!0}
!6 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!7 = !{i32 2, !"Dwarf Version", i32 4}
!8 = !{i32 2, !"Debug Info Version", i32 3}
!9 = !{i32 1, !"wchar_size", i32 4}
!10 = !{i32 7, !"PIC Level", i32 2}
!11 = !{!"clang version 8.0.0 (tags/RELEASE_800/final)"}
!12 = distinct !DISubprogram(name: "main", scope: !3, file: !3, line: 10, type: !13, scopeLine: 10, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !2, retainedNodes: !4)
!13 = !DISubroutineType(types: !14)
!14 = !{!6}
!15 = !DILocation(line: 11, column: 10, scope: !12)
!16 = !DILocation(line: 11, column: 12, scope: !12)
!17 = !DILocation(line: 11, column: 3, scope: !12)
