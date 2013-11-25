#!/usr/bin/env runhaskell

import Text.Printf
import System.IO
import System.Environment

-- | configuration
step :: Double
step = 10.0
minimal :: Double
minimal = 10.0
maximal :: Double
maximal = 100.0
maxbrightnesspath :: String
maxbrightnesspath = "/sys/devices/pci0000:00/0000:00:02.0/drm/card0"
    ++ "/card0-eDP-1/intel_backlight/max_brightness"
brightnesspath :: String
brightnesspath = "/sys/devices/pci0000:00/0000:00:02.0/drm/card0"
    ++ "/card0-eDP-1/intel_backlight/brightness"

-- | main function
main :: IO ()
main = do
    cmd <- getProgName
    args <- getArgs
    case args of
        ["up"] -> step_brightness 1
        ["down"] -> step_brightness (-1)
        _ -> putStrLn $ "usage: " ++ cmd ++ " <up|down>"

-- | helpers
read_int :: String -> (IO Int)
read_int path = withFile path ReadMode $ fmap read . hGetLine

write_int :: String -> Int -> IO ()
write_int path = withFile path WriteMode . flip hPutStr . show

edge :: Double -> Double -> Double -> Double
edge a b c = max (min b c) a

-- | core procedure
step_brightness :: Int -> IO ()
step_brightness inc = do
    max <- read_int maxbrightnesspath
    level_initial <- read_int brightnesspath
    let level :: Double
        level  = fromIntegral level_initial / fromIntegral max
        level' = level + (step * fromIntegral inc) / 100
        next_level = edge (minimal / 100) level' (maximal / 100)
    printf "%.0f%% -> %.0f%%\n" (level * 100) (next_level * 100)
    write_int brightnesspath (floor $ next_level * fromIntegral max)
